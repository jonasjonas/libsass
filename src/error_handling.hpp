#ifndef SASS_ERROR_HANDLING_H
#define SASS_ERROR_HANDLING_H

#include <string>
#include <sstream>
#include <stdexcept>
#include "position.hpp"
#include "ast_fwd_decl.hpp"
#include "sass/functions.h"

namespace Sass {

  struct Backtrace;

  namespace Exception {

    const std::string def_msg = "Invalid sass detected";
    const std::string def_op_msg = "Undefined operation";
    const std::string def_op_null_msg = "Invalid null operation";
    const std::string def_nesting_limit = "Code too deeply neested";

    class Base : public std::runtime_error {
      protected:
        std::string msg;
        std::string prefix;
      public:
        ParserState pstate;
        std::vector<Sass_Import_Entry>* import_stack;
      public:
        Base(ParserState pstate, std::string msg = def_msg, std::vector<Sass_Import_Entry>* import_stack = 0);
        virtual const char* errtype() const { return prefix.c_str(); }
        virtual const char* what() const throw() { return msg.c_str(); }
        virtual ~Base() throw() {};
    };

    class InvalidSass : public Base {
      public:
        InvalidSass(ParserState pstate, std::string msg);
        virtual ~InvalidSass() throw() {};
    };

    class InvalidParent : public Base {
      protected:
        Selector_Ptr parent;
        Selector_Ptr selector;
      public:
        InvalidParent(Selector_Ptr parent, Selector_Ptr selector);
        virtual ~InvalidParent() throw() {};
    };

    class MissingArgument : public Base {
      protected:
        std::string fn;
        std::string arg;
        std::string fntype;
      public:
        MissingArgument(ParserState pstate, std::string fn, std::string arg, std::string fntype);
        virtual ~MissingArgument() throw() {};
    };

    class InvalidArgumentType : public Base {
      protected:
        std::string fn;
        std::string arg;
        std::string type;
        const Value_Ptr value;
      public:
        InvalidArgumentType(ParserState pstate, std::string fn, std::string arg, std::string type, const Value_Ptr value = 0);
        virtual ~InvalidArgumentType() throw() {};
    };

    class InvalidVarKwdType : public Base {
      protected:
        std::string name;
        const Argument_Ptr arg;
      public:
        InvalidVarKwdType(ParserState pstate, std::string name, const Argument_Ptr arg = 0);
        virtual ~InvalidVarKwdType() throw() {};
    };

    class InvalidSyntax : public Base {
      public:
        InvalidSyntax(ParserState pstate, std::string msg, std::vector<Sass_Import_Entry>* import_stack = 0);
        virtual ~InvalidSyntax() throw() {};
    };

    class NestingLimitError : public Base {
      public:
        NestingLimitError(ParserState pstate, std::string msg = def_nesting_limit, std::vector<Sass_Import_Entry>* import_stack = 0);
        virtual ~NestingLimitError() throw() {};
    };

    /* common virtual base class (has no pstate) */
    class OperationError : public std::runtime_error {
      protected:
        std::string msg;
      public:
        OperationError(std::string msg = def_op_msg)
        : std::runtime_error(msg), msg(msg)
        {};
      public:
        virtual const char* errtype() const { return "Error"; }
        virtual const char* what() const throw() { return msg.c_str(); }
        virtual ~OperationError() throw() {};
    };

    class ZeroDivisionError : public OperationError {
      protected:
        const Expression& lhs;
        const Expression& rhs;
      public:
        ZeroDivisionError(const Expression& lhs, const Expression& rhs);
        virtual const char* errtype() const { return "ZeroDivisionError"; }
        virtual ~ZeroDivisionError() throw() {};
    };

    class DuplicateKeyError : public Base {
      protected:
        const Map& dup;
        const Expression& org;
      public:
        DuplicateKeyError(const Map& dup, const Expression& org);
        virtual const char* errtype() const { return "Error"; }
        virtual ~DuplicateKeyError() throw() {};
    };

    class TypeMismatch : public Base {
      protected:
        const Expression& var;
        const std::string type;
      public:
        TypeMismatch(const Expression& var, const std::string type);
        virtual const char* errtype() const { return "Error"; }
        virtual ~TypeMismatch() throw() {};
    };

    class InvalidValue : public Base {
      protected:
        const Expression& val;
      public:
        InvalidValue(const Expression& val);
        virtual const char* errtype() const { return "Error"; }
        virtual ~InvalidValue() throw() {};
    };

    class StackError : public Base {
      protected:
        const AST_Node& node;
      public:
        StackError(const AST_Node& node);
        virtual const char* errtype() const { return "SystemStackError"; }
        virtual ~StackError() throw() {};
    };

    class IncompatibleUnits : public OperationError {
      protected:
        // const Sass::UnitType lhs;
        // const Sass::UnitType rhs;
      public:
        IncompatibleUnits(const Number& lhs, const Number& rhs);
        IncompatibleUnits(const UnitType lhs, const UnitType rhs);
        virtual ~IncompatibleUnits() throw() {};
    };

    class UndefinedOperation : public OperationError {
      protected:
        Expression_Ptr_Const lhs;
        Expression_Ptr_Const rhs;
        const std::string op;
      public:
        UndefinedOperation(Expression_Ptr_Const lhs, Expression_Ptr_Const rhs, const std::string& op);
        // virtual const char* errtype() const { return "Error"; }
        virtual ~UndefinedOperation() throw() {};
    };

    class InvalidNullOperation : public UndefinedOperation {
      public:
        InvalidNullOperation(Expression_Ptr_Const lhs, Expression_Ptr_Const rhs, const std::string& op);
        virtual ~InvalidNullOperation() throw() {};
    };

    class AlphaChannelsNotEqual : public OperationError {
      protected:
        Expression_Ptr_Const lhs;
        Expression_Ptr_Const rhs;
        const std::string op;
      public:
        AlphaChannelsNotEqual(Expression_Ptr_Const lhs, Expression_Ptr_Const rhs, const std::string& op);
        // virtual const char* errtype() const { return "Error"; }
        virtual ~AlphaChannelsNotEqual() throw() {};
    };

    class SassValueError : public Base {
      public:
        SassValueError(ParserState pstate, OperationError& err);
        virtual ~SassValueError() throw() {};
    };

  }

  void warn(std::string msg, ParserState pstate);
  void warn(std::string msg, ParserState pstate, Backtrace* bt);

  void deprecated_function(std::string msg, ParserState pstate);
  void deprecated(std::string msg, std::string msg2, bool with_column, ParserState pstate);
  void deprecated_bind(std::string msg, ParserState pstate);
  // void deprecated(std::string msg, ParserState pstate, Backtrace* bt);

  void error(std::string msg, ParserState pstate);
  void error(std::string msg, ParserState pstate, Backtrace* bt);

}

#endif
