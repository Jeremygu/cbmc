SRC = arith_tools.cpp \
      array_name.cpp \
      base_type.cpp \
      bv_arithmetic.cpp \
      byte_operators.cpp \
      c_types.cpp \
      cmdline.cpp \
      config.cpp \
      cout_message.cpp \
      decision_procedure.cpp \
      dstring.cpp \
      endianness_map.cpp \
      expr.cpp \
      expr_util.cpp \
      file_util.cpp \
      find_macros.cpp \
      find_symbols.cpp \
      fixedbv.cpp \
      format_constant.cpp \
      fresh_symbol.cpp \
      get_base_name.cpp \
      get_module.cpp \
      graph.cpp \
      guard.cpp \
      identifier.cpp \
      ieee_float.cpp \
      irep.cpp \
      irep_hash.cpp \
      irep_hash_container.cpp \
      irep_ids.cpp \
      irep_serialization.cpp \
      json.cpp \
      json_expr.cpp \
      json_irep.cpp \
      language.cpp \
      language_file.cpp \
      lispexpr.cpp \
      lispirep.cpp \
      memory_info.cpp \
      merge_irep.cpp \
      message.cpp \
      mp_arith.cpp \
      namespace.cpp \
      options.cpp \
      parse_options.cpp \
      parser.cpp \
      pipe_stream.cpp \
      pointer_offset_size.cpp \
      pointer_predicates.cpp \
      rational.cpp \
      rational_tools.cpp \
      ref_expr_set.cpp \
      rename.cpp \
      rename_symbol.cpp \
      replace_expr.cpp \
      replace_symbol.cpp \
      run.cpp \
      signal_catcher.cpp \
      simplify_expr.cpp \
      simplify_expr_array.cpp \
      simplify_expr_boolean.cpp \
      simplify_expr_floatbv.cpp \
      simplify_expr_int.cpp \
      simplify_expr_pointer.cpp \
      simplify_expr_struct.cpp \
      simplify_utils.cpp \
      source_location.cpp \
      ssa_expr.cpp \
      std_code.cpp \
      std_expr.cpp \
      std_types.cpp \
      string2int.cpp \
      string_container.cpp \
      string_hash.cpp \
      string_utils.cpp \
      substitute.cpp \
      symbol.cpp \
      symbol_table.cpp \
      tempdir.cpp \
      tempfile.cpp \
      threeval.cpp \
      time_stopping.cpp \
      timer.cpp \
      type.cpp \
      type_eq.cpp \
      typecheck.cpp \
      ui_message.cpp \
      unicode.cpp \
      union_find.cpp \
      xml.cpp \
      xml_expr.cpp \
      xml_irep.cpp \
      # Empty last line

INCLUDES= -I ..

include ../config.inc
include ../common

CLEANFILES = util$(LIBEXT)

all: util$(LIBEXT)

util$(LIBEXT): $(OBJ)
	$(LINKLIB)
