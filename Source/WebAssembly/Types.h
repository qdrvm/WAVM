#pragma once

#include "Core/Core.h"
#include "WebAssembly.h"

namespace WebAssembly
{
	enum class ValueType : uint8
	{
		invalid = 0,
		i32 = 1,
		i64 = 2,
		f32 = 3,
		f64 = 4,

		max = 4,
		num = max + 1
	};

	template<ValueType type> struct ValueTypeInfo;
	template<> struct ValueTypeInfo<ValueType::i32> { typedef int32 Value; };
	template<> struct ValueTypeInfo<ValueType::i64> { typedef int64 Value; };
	template<> struct ValueTypeInfo<ValueType::f32> { typedef float32 Value; };
	template<> struct ValueTypeInfo<ValueType::f64> { typedef float64 Value; };

	enum class ReturnType : uint8
	{
		invalid = (uint8)ValueType::invalid,
		i32 = (uint8)ValueType::i32,
		i64 = (uint8)ValueType::i64,
		f32 = (uint8)ValueType::f32,
		f64 = (uint8)ValueType::f64,
		unit = (uint8)ValueType::max + 1,

		max = unit,
		num = max + 1
	};

	struct FunctionType
	{
		ReturnType ret;
		std::vector<ValueType> parameters;

		WEBASSEMBLY_API static const FunctionType* get(ReturnType ret,const std::initializer_list<ValueType>& parameters);
		WEBASSEMBLY_API static const FunctionType* get(ReturnType ret,const std::vector<ValueType>& parameters);
		WEBASSEMBLY_API static const FunctionType* get(ReturnType ret = ReturnType::unit);

	private:

		FunctionType(ReturnType inRet,const std::vector<ValueType>& inParameters)
		: ret(inRet), parameters(inParameters) {}
	};
	
	enum class TableElementType : uint8
	{
		anyfunc = 0x20
	};
	
	struct SizeConstraints
	{
		uint64 min;
		uint64 max;

		friend bool operator==(const SizeConstraints& left,const SizeConstraints& right) { return left.min == right.min && left.max == right.max; }
		friend bool operator!=(const SizeConstraints& left,const SizeConstraints& right) { return left.min != right.min || left.max != right.max; }
		friend bool isSubset(const SizeConstraints& super,const SizeConstraints& sub)
		{
			return sub.min >= super.min && sub.max <= super.max;
		}
	};

	struct TableType
	{
		TableElementType elementType;
		SizeConstraints size;
		TableType(): elementType(TableElementType::anyfunc), size({0,UINT64_MAX}) {}
		TableType(TableElementType inElementType,SizeConstraints inSize): elementType(inElementType), size(inSize) {}

		friend bool operator==(const TableType& left,const TableType& right) { return left.elementType == right.elementType && left.size == right.size; }
		friend bool operator!=(const TableType& left,const TableType& right) { return left.elementType != right.elementType || left.size != right.size; }
	};

	struct MemoryType
	{
		SizeConstraints size;
		
		MemoryType(): size({0,UINT64_MAX}) {}
		MemoryType(const SizeConstraints& inSize): size(inSize) {}

		friend bool operator==(const MemoryType& left,const MemoryType& right) { return left.size == right.size; }
		friend bool operator!=(const MemoryType& left,const MemoryType& right) { return left.size != right.size; }
	};

	struct GlobalType
	{
		ValueType valueType;
		bool isMutable;
		
		friend bool operator==(const GlobalType& left,const GlobalType& right) { return left.valueType == right.valueType && left.isMutable == right.isMutable; }
		friend bool operator!=(const GlobalType& left,const GlobalType& right) { return left.valueType != right.valueType || left.isMutable != right.isMutable; }
	};
	
	enum class ObjectKind : uint8
	{
		function = 0,
		table = 1,
		memory = 2,
		global = 3,
		module = 4,
		max = 4,
		invalid = 0xff
	};

	struct ObjectType
	{
		ObjectKind kind;
		union
		{
			const FunctionType* function;
			TableType table;
			MemoryType memory;
			GlobalType global;
		};
		ObjectType()							: kind(ObjectKind::invalid) {}
		ObjectType(const FunctionType* inFunction)	: kind(ObjectKind::function), function(inFunction) {}
		ObjectType(TableType inTable)			: kind(ObjectKind::table), table(inTable) {}
		ObjectType(MemoryType inMemory)		: kind(ObjectKind::memory), memory(inMemory) {}
		ObjectType(GlobalType inGlobal)			: kind(ObjectKind::global), global(inGlobal) {}
	};
	
	inline size_t getArity(ReturnType returnType) { return returnType == ReturnType::unit ? 0 : 1; }
	
	inline ValueType asValueType(ReturnType type)
	{
		assert(type != ReturnType::unit);
		return (ValueType)type;
	}

	inline ReturnType asReturnType(ValueType type)
	{
		assert(type != ValueType::invalid);
		return (ReturnType)type;
	}

	inline const char* getTypeName(ValueType type)
	{
		switch(type)
		{
		case ValueType::invalid: return "invalid";
		case ValueType::i32: return "i32";
		case ValueType::i64: return "i64";
		case ValueType::f32: return "f32";
		case ValueType::f64: return "f64";
		default: return "unknown";
		};
	}

	inline const char* getTypeName(ReturnType type)
	{
		switch(type)
		{
		case ReturnType::unit: return "()";
		case ReturnType::i32: return "i32";
		case ReturnType::i64: return "i64";
		case ReturnType::f32: return "f32";
		case ReturnType::f64: return "f64";
		default: return "unknown";
		};
	}

	inline std::string getTypeName(const std::vector<ValueType>& typeTuple)
	{
		std::string result = "(";
		for(uintptr parameterIndex = 0;parameterIndex < typeTuple.size();++parameterIndex)
		{
			if(parameterIndex != 0) { result += ','; }
			result += getTypeName(typeTuple[parameterIndex]);
		}
		result += ")";
		return result;
	}

	inline std::string getTypeName(const FunctionType* functionType)
	{
		return getTypeName(functionType->parameters) + "->" + getTypeName(functionType->ret);
	}

	inline std::string getTypeName(const GlobalType& globalType)
	{
		if(globalType.isMutable) { return std::string("global") + getTypeName(globalType.valueType); }
		else { return std::string("immutable") + getTypeName(globalType.valueType); }
	}

	inline std::string getTypeName(const ObjectType& objectType)
	{
		switch(objectType.kind)
		{
		case ObjectKind::function: return "func " + getTypeName(objectType.function);
		case ObjectKind::table: return "table";
		case ObjectKind::memory: return "memory";
		case ObjectKind::global: return getTypeName(objectType.global);
		default: throw;
		};
	}

	inline uint8 getTypeBitWidth(ValueType type)
	{
		switch(type)
		{
		case ValueType::i32: return 32;
		case ValueType::i64: return 64;
		case ValueType::f32: return 32;
		case ValueType::f64: return 64;
		default: throw;
		};
	}
}