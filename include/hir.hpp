#pragma once

#include <map>
#include <memory>
#include <string>
#include <tuple>
#include <utility>
#include <vector>
#include <cstdint>
namespace HIR {
using namespace std;

struct Module;
struct Function;
struct FunctionProtype;
struct BasicBlock;
struct Value;


enum basetype {
	i32,
	f32,
};
struct datatype {
	bool isVoid_;
	bool isPointer_;
	bool isArray_;
	int arraysize_;

	basetype basetype_;
	std::vector<int32_t> dims; //We can use smallvector to improve performance

	static datatype getType(basetype basetype_);
	static datatype voidType();
	static datatype getPointerType(basetype basetype_, std::vector<int32_t> dims);
	static datatype getArrayType(basetype basetype_,std::vector<int32_t> dims);

	bool isMeta(){ return isPointer_ == false && isArray_ == false && isVoid_ == false; }
	bool isI32(){return isMeta() && basetype_ == basetype::i32;}
	bool isF32(){return isMeta() && basetype_ == basetype::f32;}
};

using vtag = tuple<int,string,string>;

struct Module {
	map<string,FunctionProtype> func_decl;
	map<string,Function> func_define;

	map<string,datatype> globals_;
	map<vtag,pair<datatype,void*>> constants_;
};

enum class ReturnType{
	i32,
	f32,
	void_,
};
struct FunctionProtype {
	ReturnType type;
	vector<datatype> params;
};

struct Function {
	FunctionProtype proty_;
	vector<shared_ptr<Value>> params_;
	vector<shared_ptr<BasicBlock>> BBs_;
	vector<shared_ptr<Value>> values_;

	Module& module;
};

struct BasicBlock {
	vector<Value*> ops;
	Function& func;
};

struct Value {
	datatype dt_;
	int no;
	enum ValueKind {
		ConstInt,
		ConstFLoat,
		Int2Float,
		Float2Int,
		LocalVarDef,
		GlobalVar,
		IBinOp,
		FBinOp,
		Jump,
		CondJump,
		FuncCall,
		Return,
		Alloc,
		Store,
		GetPtr,
		Assign,
	} Kind ;
	virtual ~Value() = 0;
};


struct ConstInt : Value {
	int v;
	ConstInt(int v) : v(v) {
		Kind = ValueKind::ConstInt;
		dt_ = datatype::getType(basetype::i32);
	}
};
struct ConstFloat : Value {
	float v;
	ConstFloat(float v) : v(v) {
		Kind = ValueKind::ConstFLoat;
		dt_ = datatype::getType(basetype::f32);
	}
};

struct Int2Float : Value {
	Value* v;
	Int2Float(Value *v) : v(v){
		Kind = ValueKind::Int2Float;
		dt_ = datatype::getType(basetype::f32);
	}
};
struct Float2Int : Value {
	Value* v;
	Float2Int(Value *v) : v(v){
		Kind = ValueKind::Float2Int;
		dt_ = datatype::getType(basetype::i32);
	}
};



struct Builder {

	Value* CreateInt2Float(Value *v){

	}
};

}