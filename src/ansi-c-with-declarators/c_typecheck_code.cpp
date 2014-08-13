/*******************************************************************\

Module: C Language Type Checking

Author: Daniel Kroening, kroening@kroening.com

\*******************************************************************/

#include <util/config.h>
#include <linking/zero_initializer.h>

#include "ansi_c_declaration.h"
#include "c_typecheck_base.h"

/*******************************************************************\

Function: c_typecheck_baset::init

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

void c_typecheck_baset::start_typecheck_code()
{
  case_is_allowed=break_is_allowed=continue_is_allowed=false;
}

/*******************************************************************\

Function: c_typecheck_baset::typecheck_code

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

void c_typecheck_baset::typecheck_code(codet &code)
{
  if(code.id()!=ID_code)
    throw "expected code, got "+code.pretty();

  code.type()=code_typet();

  const irep_idt &statement=code.get_statement();

  if(statement==ID_expression)
    typecheck_expression(code);
  else if(statement==ID_label)
    typecheck_label(to_code_label(code));
  else if(statement==ID_switch_case)
    typecheck_switch_case(to_code_switch_case(code));
  else if(statement==ID_gcc_switch_case_range)
    typecheck_gcc_switch_case_range(code);
  else if(statement==ID_block)
    typecheck_block(code);
  else if(statement==ID_ifthenelse)
    typecheck_ifthenelse(to_code_ifthenelse(code));
  else if(statement==ID_while)
    typecheck_while(to_code_while(code));
  else if(statement==ID_dowhile)
    typecheck_dowhile(to_code_dowhile(code));
  else if(statement==ID_for)
    typecheck_for(code);
  else if(statement==ID_switch)
    typecheck_switch(to_code_switch(code));
  else if(statement==ID_break)
    typecheck_break(code);
  else if(statement==ID_goto)
    typecheck_goto(code);
  else if(statement=="computed-goto")
    typecheck_computed_goto(code);
  else if(statement==ID_continue)
    typecheck_continue(code);
  else if(statement==ID_return)
    typecheck_return(code);
  else if(statement==ID_decl)
    typecheck_decl(code);
  else if(statement==ID_decl_type)
    typecheck_decl_type(code);
  else if(statement==ID_assign)
    typecheck_assign(code);
  else if(statement==ID_skip)
  {
  }
  else if(statement==ID_asm)
    typecheck_asm(code);
  else if(statement==ID_start_thread)
    typecheck_start_thread(code);
  else if(statement==ID_gcc_local_label)
    typecheck_gcc_local_label(code);
  else if(statement==ID_msc_try_finally)
  {
    assert(code.operands().size()==2);
    typecheck_code(to_code(code.op0()));
    typecheck_code(to_code(code.op1()));
  }
  else if(statement==ID_msc_try_except)
  {
    assert(code.operands().size()==3);
    typecheck_code(to_code(code.op0()));
    typecheck_expr(code.op1());
    typecheck_code(to_code(code.op2()));
  }
  else if(statement==ID_msc_leave)
  {
    // fine as is, but should check that we
    // are in a 'try' block
  }
  else if(statement==ID_static_assert)
  {
    assert(code.operands().size()==2);
    typecheck_expr(code.op0());
    typecheck_expr(code.op1());
  }
  else if(statement==ID_CPROVER_try_catch ||
          statement==ID_CPROVER_try_finally)
  {
    assert(code.operands().size()==2);
    typecheck_code(to_code(code.op0()));
    typecheck_code(to_code(code.op1()));
  }
  else if(statement==ID_CPROVER_throw)
  {
    assert(code.operands().size()==0);
  }
  else if(statement==ID_assume ||
          statement==ID_assert)
  {
    // These are not generated by the C/C++ parsers,
    // but we allow them for the benefit of other users
    // of the typechecker.
    assert(code.operands().size()==1);
    typecheck_expr(code.op0());
  }
  else
  {
    err_location(code);
    str << "unexpected statement: " << statement;
    throw 0;
  }
}

/*******************************************************************\

Function: c_typecheck_baset::typecheck_asm

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

void c_typecheck_baset::typecheck_asm(codet &code)
{
}

/*******************************************************************\

Function: c_typecheck_baset::typecheck_assign

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

void c_typecheck_baset::typecheck_assign(codet &code)
{
  if(code.operands().size()!=2)
    throw "assignment statement expected to have two operands";

  typecheck_expr(code.op0());
  typecheck_expr(code.op1());

  implicit_typecast(code.op1(), code.op0().type());
}

/*******************************************************************\

Function: c_typecheck_baset::typecheck_block

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

void c_typecheck_baset::typecheck_block(codet &code)
{
  Forall_operands(it, code)
    typecheck_code(to_code(*it));

  // do decl-blocks

  exprt new_ops;
  new_ops.operands().reserve(code.operands().size());

  Forall_operands(it1, code)
  {
    if(it1->is_nil()) continue;

    codet &code_op=to_code(*it1);
  
    if(code_op.get_statement()==ID_label)
    {
      // these may be nested
      codet *code_ptr=&code_op;
      
      while(code_ptr->get_statement()==ID_label)
      {
        assert(code_ptr->operands().size()==1);
        code_ptr=&to_code(code_ptr->op0());
      }
      
      codet &label_op=*code_ptr;

      new_ops.move_to_operands(code_op);
    }
    else
      new_ops.move_to_operands(code_op);
  }

  code.operands().swap(new_ops.operands());
}

/*******************************************************************\

Function: c_typecheck_baset::typecheck_break

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

void c_typecheck_baset::typecheck_break(codet &code)
{
  if(!break_is_allowed)
  {
    err_location(code);
    throw "break not allowed here";
  }
}

/*******************************************************************\

Function: c_typecheck_baset::typecheck_continue

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

void c_typecheck_baset::typecheck_continue(codet &code)
{
  if(!continue_is_allowed)
  {
    err_location(code);
    throw "continue not allowed here";
  }
}

/*******************************************************************\

Function: c_typecheck_baset::typecheck_decl

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

void c_typecheck_baset::typecheck_decl_type(codet &code)
{
  assert(code.operands().size()==0);
  // Type only! May have side-effects in it!
  
  std::list<codet> clean_code;
  
  typet &type=static_cast<typet &>(code.add(ID_type_arg));

  // typecheck
  typecheck_type(type);

  // clean
  clean_type(irep_idt(), type, clean_code);
  
  if(!clean_code.empty())
  {
    // build a decl-block
    code_blockt code_block(clean_code);
    code_block.set_statement(ID_decl_block);
    code_block.copy_to_operands(code);
    code.swap(code_block);
  }
}

/*******************************************************************\

Function: c_typecheck_baset::typecheck_decl

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

void c_typecheck_baset::typecheck_decl(codet &code)
{
  // this comes with 1 operand, which is a declaration
  if(code.operands().size()!=1)
  {
    err_location(code);
    throw "decl expected to have 1 operand";
  }

  // op0 must be declaration
  if(code.op0().id()!=ID_declaration)
  {
    err_location(code);
    throw "decl statement expected to have declaration as operand";
  }

  ansi_c_declarationt declaration;
  declaration.swap(code.op0());
  
  typecheck_declaration(declaration);
  
  std::list<codet> new_code;
  
  // iterate over declarators
  
  for(ansi_c_declarationt::declaratorst::const_iterator
      d_it=declaration.declarators().begin();
      d_it!=declaration.declarators().end();
      d_it++)
  {
    // add prefix
    irep_idt identifier=
      add_language_prefix(d_it->get_name());

    // look it up
    symbol_tablet::symbolst::iterator s_it=
      symbol_table.symbols.find(identifier);

    if(s_it==symbol_table.symbols.end())
    {
      err_location(code);
      str << "failed to find decl symbol `" << identifier
          << "' in symbol table";
      throw 0;
    }

    symbolt &symbol=s_it->second;
    
    // There may be side-effects in the type.  
    clean_type(symbol.name, symbol.type, new_code);

    // this must not be an incomplete type
    if(!is_complete_type(symbol.type))
    {
      err_location(symbol.location);
      throw "incomplete type not permitted here";
    }
  
    // see if it's a typedef
    // or a function
    // or static
    if(symbol.is_type ||
       symbol.type.id()==ID_code ||
       symbol.is_static_lifetime)
    {
      // we ignore
    }
    else
    {
      code_declt code;
      code.location()=symbol.location;
      code.symbol()=symbol.symbol_expr();
      code.symbol().location()=symbol.location;
      
      // add initializer, if any
      if(symbol.value.is_not_nil())
      {
        code.operands().resize(2);
        code.op1()=symbol.value; 
      }
      
      new_code.push_back(code);
    }
  }
  
  if(new_code.empty())
  {
    locationt location=code.location();
    code=code_skipt();
    code.location()=location;
  }
  else if(new_code.size()==1)
  {
    code.swap(new_code.front());
  }
  else
  {
    // build a decl-block
    code_blockt code_block(new_code);
    code_block.set_statement(ID_decl_block);
    code.swap(code_block);
  }
}

/*******************************************************************\

Function: move_declarations

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

static void move_declarations(
  exprt &code,
  std::list<codet> &clean_code)
{
  Forall_operands(it, code)
    move_declarations(*it, clean_code);

  if(code.id()==ID_code &&
     to_code(code).get_statement()==ID_decl)
  {
    clean_code.push_back(code_skipt());
    code.swap(clean_code.back());
  }
}

/*******************************************************************\

Function: c_typecheck_baset::is_complete_type

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

bool c_typecheck_baset::is_complete_type(const typet &type) const
{
  if(type.id()==ID_incomplete_struct ||
     type.id()==ID_incomplete_union)
    return false;
  else if(type.id()==ID_array)
  {
    if(to_array_type(type).size().is_nil()) return false;
    return is_complete_type(type.subtype());
  }
  else if(type.id()==ID_struct || type.id()==ID_union)
  {
    const struct_union_typet::componentst &components=
      to_struct_union_type(type).components();
    for(struct_union_typet::componentst::const_iterator
        it=components.begin();
        it!=components.end();
        it++)
      if(!is_complete_type(it->type()))
        return false;
  }
  else if(type.id()==ID_vector)
    return is_complete_type(type.subtype());
  else if(type.id()==ID_symbol)
    return is_complete_type(follow(type));
  
  return true;
}

/*******************************************************************\

Function: c_typecheck_baset::typecheck_expression

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

void c_typecheck_baset::typecheck_expression(codet &code)
{
  if(code.operands().size()!=1)
    throw "expression statement expected to have one operand";

  exprt &op=code.op0();
  typecheck_expr(op);

  #if 0
  // Goes away since stuff like x=({y=1;});
  // needs the inner side-effect.
  if(op.id()==ID_sideeffect)
  {
    const irep_idt &statement=op.get(ID_statement);
    
    if(statement==ID_assign)
    {
      assert(op.operands().size()==2);
      
      // pull assignment statements up
      exprt::operandst operands;
      operands.swap(op.operands());
      code.set_statement(ID_assign);
      code.operands().swap(operands);
      code.location()=op.location();
      
      if(code.op1().id()==ID_sideeffect &&
         code.op1().get(ID_statement)==ID_function_call)
      {
        assert(code.op1().operands().size()==2);
  
        code_function_callt function_call;
        function_call.location().swap(code.op1().location());
        function_call.lhs()=code.op0();
        function_call.function()=code.op1().op0();
        function_call.arguments()=code.op1().op1().operands();
        code.swap(function_call);
      }
    }
    else if(statement==ID_function_call)
    {
      assert(op.operands().size()==2);
      
      // pull function calls up
      code_function_callt function_call;
      function_call.location()=code.location();
      function_call.function()=op.op0();
      function_call.arguments()=op.op1().operands();
      code.swap(function_call);
    }
  }
  #endif
}

/*******************************************************************\

Function: c_typecheck_baset::typecheck_for

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

void c_typecheck_baset::typecheck_for(codet &code)
{
  if(code.operands().size()!=4)
    throw "for expected to have four operands";
    
  // the "for" statement has an implicit block around it,
  // since code.op0() may contain declarations
  //
  // we therefore transform
  //
  //   for(a;b;c) d;
  //
  // to
  //
  //   { a; for(;b;c) d; }
  //
  // if config.ansi_c.for_has_scope

  if(!config.ansi_c.for_has_scope ||
     code.op0().is_nil())
  {
    if(code.op0().is_not_nil())
      typecheck_code(to_code(code.op0()));

    if(code.op1().is_nil())
      code.op1()=true_exprt();
    else
    {
      typecheck_expr(code.op1());
      implicit_typecast_bool(code.op1());
    }

    if(code.op2().is_not_nil())
      typecheck_expr(code.op2());

    if(code.op3().is_not_nil())
    {
      // save & set flags
      bool old_break_is_allowed=break_is_allowed;
      bool old_continue_is_allowed=continue_is_allowed;

      break_is_allowed=continue_is_allowed=true;

      // recursive call
      if(to_code(code.op3()).get_statement()==ID_decl_block)
      {
        code_blockt code_block;
        code_block.location()=code.op3().location();

        code_block.move_to_operands(code.op3());
        code.op3().swap(code_block);
      }
      typecheck_code(to_code(code.op3()));

      // restore flags
      break_is_allowed=old_break_is_allowed;
      continue_is_allowed=old_continue_is_allowed;
    }
  }
  else
  {
    code_blockt code_block;
    code_block.location()=code.location();
    if(to_code(code.op3()).get_statement()==ID_block)
      code_block.set(
        ID_C_end_location,
        to_code_block(to_code(code.op3())).end_location());
    else
      code_block.set(
        ID_C_end_location,
        code.op3().location());;

    code_block.reserve_operands(2);
    code_block.move_to_operands(code.op0());
    code.op0().make_nil();
    code_block.move_to_operands(code);
    code.swap(code_block);
    typecheck_code(code); // recursive call
  }
}

/*******************************************************************\

Function: c_typecheck_baset::typecheck_label

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

void c_typecheck_baset::typecheck_label(code_labelt &code)
{
  if(code.operands().size()!=1)
  {
    err_location(code);
    throw "label expected to have one operand";
  }

  typecheck_code(code.code());
}

/*******************************************************************\

Function: c_typecheck_baset::typecheck_switch_case

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

void c_typecheck_baset::typecheck_switch_case(code_switch_caset &code)
{
  if(code.operands().size()!=2)
  {
    err_location(code);
    throw "switch_case expected to have two operands";
  }

  typecheck_code(code.code());

  if(code.is_default())
  {
    if(!case_is_allowed)
    {
      err_location(code);
      throw "did not expect default label here";
    }
  }
  else
  {
    if(!case_is_allowed)
    {
      err_location(code);
      throw "did not expect `case' here";
    }
  
    exprt &case_expr=code.case_op();
    typecheck_expr(case_expr);
    implicit_typecast(case_expr, switch_op_type);
  }
}

/*******************************************************************\

Function: c_typecheck_baset::typecheck_gcc_switch_case_range

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

void c_typecheck_baset::typecheck_gcc_switch_case_range(codet &code)
{
  if(code.operands().size()!=3)
  {
    err_location(code);
    throw "gcc_switch_case_range expected to have three operands";
  }

  typecheck_code(to_code(code.op2()));

  if(!case_is_allowed)
  {
    err_location(code);
    throw "did not expect `case' here";
  }

  typecheck_expr(code.op0());
  typecheck_expr(code.op1());
  implicit_typecast(code.op0(), switch_op_type);
  implicit_typecast(code.op1(), switch_op_type);
}

/*******************************************************************\

Function: c_typecheck_baset::typecheck_gcc_local_label

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

void c_typecheck_baset::typecheck_gcc_local_label(codet &code)
{
  // these are just declarations, e.g.,
  // __label__ here, there;
}

/*******************************************************************\

Function: c_typecheck_baset::typecheck_goto

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

void c_typecheck_baset::typecheck_goto(codet &code)
{
}

/*******************************************************************\

Function: c_typecheck_baset::typecheck_computed_goto

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

void c_typecheck_baset::typecheck_computed_goto(codet &code)
{
  if(code.operands().size()!=1)
  {
    err_location(code);
    throw "computed-goto expected to have one operand";
  }

  exprt &dest=code.op0();
  
  if(dest.id()!=ID_dereference)
  {
    err_location(dest);
    throw "computed-goto expected to have dereferencing operand";
  }
  
  assert(dest.operands().size()==1);

  typecheck_expr(dest.op0());
  dest.type()=empty_typet();
}

/*******************************************************************\

Function: c_typecheck_baset::typecheck_ifthenelse

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

void c_typecheck_baset::typecheck_ifthenelse(code_ifthenelset &code)
{
  if(code.operands().size()!=3)
    throw "ifthenelse expected to have three operands";

  exprt &cond=code.cond();

  typecheck_expr(cond);

  #if 0
  if(cond.id()==ID_sideeffect &&
     cond.get(ID_statement)==ID_assign)
  {
    err_location(cond);
    warning("warning: assignment in if condition");
  }
  #endif

  implicit_typecast_bool(cond);

  if(to_code(code.then_case()).get_statement()==ID_decl_block)
  {
    code_blockt code_block;
    code_block.location()=code.then_case().location();

    code_block.move_to_operands(code.then_case());
    code.then_case().swap(code_block);
  }
  
  typecheck_code(to_code(code.then_case()));

  if(!code.else_case().is_nil())
  {
    if(to_code(code.else_case()).get_statement()==ID_decl_block)
    {
      code_blockt code_block;
      code_block.location()=code.else_case().location();

      code_block.move_to_operands(code.else_case());
      code.else_case().swap(code_block);
    }

    typecheck_code(to_code(code.else_case()));
  }
}

/*******************************************************************\

Function: c_typecheck_baset::typecheck_start_thread

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

void c_typecheck_baset::typecheck_start_thread(codet &code)
{
  if(code.operands().size()!=1)
    throw "start_thread expected to have one operand";

  typecheck_code(to_code(code.op0()));
}

/*******************************************************************\

Function: c_typecheck_baset::typecheck_return

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

void c_typecheck_baset::typecheck_return(codet &code)
{
  if(code.operands().size()==0)
  {
    if(follow(return_type).id()!=ID_empty)
    {
      // gcc doesn't actually complain, it just warns!
      // We'll put a zero here, which is dubious.
      exprt zero=zero_initializer(return_type, code.location(), *this, get_message_handler());
      code.copy_to_operands(zero);
    }
  }
  else if(code.operands().size()==1)
  {
    typecheck_expr(code.op0());

    if(follow(return_type).id()==ID_empty)
    {
      // gcc doesn't actually complain, it just warns!
      if(follow(code.op0().type()).id()!=ID_empty)
        code.op0().make_typecast(return_type);
    }
    else
      implicit_typecast(code.op0(), return_type);
  }
  else
  {
    err_location(code);
    throw "return expected to have 0 or 1 operands";
  }
}

/*******************************************************************\

Function: c_typecheck_baset::typecheck_switch

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

void c_typecheck_baset::typecheck_switch(code_switcht &code)
{
  if(code.operands().size()!=2)
    throw "switch expects two operands";

  typecheck_expr(code.value());

  // this needs to be promoted
  implicit_typecast_arithmetic(code.value());

  // save & set flags

  bool old_case_is_allowed(case_is_allowed);
  bool old_break_is_allowed(break_is_allowed);
  typet old_switch_op_type(switch_op_type);

  switch_op_type=code.value().type();
  break_is_allowed=case_is_allowed=true;

  typecheck_code(code.body());

  // restore flags
  case_is_allowed=old_case_is_allowed;
  break_is_allowed=old_break_is_allowed;
  switch_op_type=old_switch_op_type;
}

/*******************************************************************\

Function: c_typecheck_baset::typecheck_while

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

void c_typecheck_baset::typecheck_while(code_whilet &code)
{
  if(code.operands().size()!=2)
    throw "while expected to have two operands";

  typecheck_expr(code.cond());
  implicit_typecast_bool(code.cond());

  // save & set flags
  bool old_break_is_allowed(break_is_allowed);
  bool old_continue_is_allowed(continue_is_allowed);

  break_is_allowed=continue_is_allowed=true;

  if(code.body().get_statement()==ID_decl_block)
  {
    code_blockt code_block;
    code_block.location()=code.body().location();

    code_block.move_to_operands(code.body());
    code.body().swap(code_block);
  }
  typecheck_code(code.body());

  // restore flags
  break_is_allowed=old_break_is_allowed;
  continue_is_allowed=old_continue_is_allowed;
}

/*******************************************************************\

Function: c_typecheck_baset::typecheck_dowhile

  Inputs:

 Outputs:

 Purpose:

\*******************************************************************/

void c_typecheck_baset::typecheck_dowhile(code_dowhilet &code)
{
  if(code.operands().size()!=2)
    throw "do while expected to have two operands";

  typecheck_expr(code.cond());
  implicit_typecast_bool(code.cond());

  // save & set flags
  bool old_break_is_allowed(break_is_allowed);
  bool old_continue_is_allowed(continue_is_allowed);

  break_is_allowed=continue_is_allowed=true;

  if(code.body().get_statement()==ID_decl_block)
  {
    code_blockt code_block;
    code_block.location()=code.body().location();

    code_block.move_to_operands(code.body());
    code.body().swap(code_block);
  }
  typecheck_code(code.body());

  // restore flags
  break_is_allowed=old_break_is_allowed;
  continue_is_allowed=old_continue_is_allowed;
}