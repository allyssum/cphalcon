/*
** 2000-05-29
**
** The author disclaims copyright to this source code.  In place of
** a legal notice, here is a blessing:
**
**    May you do good and not evil.
**    May you find forgiveness for yourself and forgive others.
**    May you share freely, never taking more than you give.
**
*************************************************************************
** Driver template for the LEMON parser generator.
**
** The "lemon" program processes an LALR(1) input grammar file, then uses
** this template to construct a parser.  The "lemon" program inserts text
** at each "%%" line.  Also, any "P-a-r-s-e" identifer prefix (without the
** interstitial "-" characters) contained in this template is changed into
** the value of the %name directive from the grammar.  Otherwise, the content
** of this template is copied straight through into the generate parser
** source file.
**
** The following is the concatenation of all %include directives from the
** input grammar file:
*/
#include <stdio.h>
/************ Begin %include sections from the grammar ************************/
/* #line 57 "parser.y" */


#include "php_phalcon.h"

#include "mvc/model/query/parser.h"
#include "mvc/model/query/scanner.h"
#include "mvc/model/query/phql.h"
#include "mvc/model/exception.h"

#include "kernel/main.h"
#include "kernel/memory.h"
#include "kernel/fcall.h"
#include "kernel/exception.h"
#include "kernel/array.h"

#include "kernel/framework/orm.h"

#include "interned-strings.h"

static zval *phql_ret_literal_zval(int type, phql_parser_token *T)
{
	zval *ret;

	MAKE_STD_ZVAL(ret);
	array_init_size(ret, 2);
	add_assoc_long(ret, phalcon_interned_type, type);
	if (T) {
		add_assoc_stringl(ret, phalcon_interned_value, T->token, T->token_len, 0);
		efree(T);
	}

	return ret;
}

static zval *phql_ret_placeholder_zval(int type, phql_parser_token *T)
{
	zval *ret;

	MAKE_STD_ZVAL(ret);
	array_init_size(ret, 2);
	add_assoc_long(ret, phalcon_interned_type, type);
	add_assoc_stringl(ret, phalcon_interned_value, T->token, T->token_len, 0);
	efree(T);

	return ret;
}

static zval *phql_ret_qualified_name(phql_parser_token *A, phql_parser_token *B, phql_parser_token *C)
{
	zval *ret;

	MAKE_STD_ZVAL(ret);
	array_init(ret);

	add_assoc_long(ret, phalcon_interned_type, PHQL_T_QUALIFIED);

	if (A != NULL) {
		add_assoc_stringl(ret, phalcon_interned_ns_alias, A->token, A->token_len, 0);
		efree(A);
	}

	if (B != NULL) {
		add_assoc_stringl(ret, phalcon_interned_domain, B->token, B->token_len, 0);
		efree(B);
	}

	add_assoc_stringl(ret, phalcon_interned_name, C->token, C->token_len, 0);
	efree(C);

	return ret;
}

static zval *phql_ret_raw_qualified_name(phql_parser_token *A, phql_parser_token *B)
{
	zval *ret;

	MAKE_STD_ZVAL(ret);
	array_init(ret);

	add_assoc_long(ret, phalcon_interned_type, PHQL_T_RAW_QUALIFIED);
	if (B != NULL) {
		add_assoc_stringl(ret, phalcon_interned_domain, A->token, A->token_len, 0);
		add_assoc_stringl(ret, phalcon_interned_name, B->token, B->token_len, 0);
		efree(B);
	} else {
		add_assoc_stringl(ret, phalcon_interned_name, A->token, A->token_len, 0);
	}
	efree(A);

	return ret;
}

static zval *phql_ret_select_statement(zval *S, zval *W, zval *O, zval *G, zval *H, zval *L, zval *F)
{
	zval *ret;

	MAKE_STD_ZVAL(ret);
	array_init(ret);

	add_assoc_long(ret, phalcon_interned_type, PHQL_T_SELECT);
	add_assoc_zval(ret, phalcon_interned_select, S);

	if (W != NULL) {
		add_assoc_zval(ret, phalcon_interned_where, W);
	}
	if (O != NULL) {
		add_assoc_zval(ret, phalcon_interned_orderBy, O);
	}
	if (G != NULL) {
		add_assoc_zval(ret, phalcon_interned_groupBy, G);
	}
	if (H != NULL) {
		add_assoc_zval(ret, phalcon_interned_having, H);
	}
	if (L != NULL) {
		add_assoc_zval(ret, phalcon_interned_limit, L);
	}
	if (F != NULL) {
		add_assoc_zval(ret, phalcon_interned_forupdate, F);
	}

	return ret;
}

static zval *phql_ret_select_clause(zval *distinct, zval *columns, zval *tables, zval *join_list, zval *force_index)
{
	zval *ret;

	MAKE_STD_ZVAL(ret);
	array_init_size(ret, 4);

	if (distinct) {
		add_assoc_zval(ret, phalcon_interned_distinct, distinct);
	}

	add_assoc_zval(ret, phalcon_interned_columns, columns);
	add_assoc_zval(ret, phalcon_interned_tables, tables);
	if (join_list) {
		add_assoc_zval(ret, phalcon_interned_joins, join_list);
	}

	if (force_index) {
		add_assoc_zval(ret, phalcon_interned_forceIndex, force_index);
	}

	return ret;
}

static zval *phql_ret_distinct_all(int distinct)
{
	zval *ret;

	MAKE_STD_ZVAL(ret);
	ZVAL_LONG(ret, distinct);

	return ret;
}

static zval *phql_ret_distinct(void)
{
	zval *ret;

	MAKE_STD_ZVAL(ret);
	ZVAL_TRUE(ret);

	return ret;
}

static zval *phql_ret_order_item(zval *column, int sort){

	zval *ret;

	MAKE_STD_ZVAL(ret);
	array_init(ret);
	add_assoc_zval(ret, phalcon_interned_column, column);
	if (sort != 0 ) {
		add_assoc_long(ret, phalcon_interned_sort, sort);
	}

	return ret;
}

static zval *phql_ret_limit_clause(zval *L, zval *O)
{
	zval *ret;

	MAKE_STD_ZVAL(ret);
	array_init_size(ret, 2);

	add_assoc_zval(ret, phalcon_interned_number, L);

	if (O != NULL) {
		add_assoc_zval(ret, phalcon_interned_offset, O);
	}

	return ret;
}

static zval *phql_ret_forupdate_clause()
{
	zval *ret;

	MAKE_STD_ZVAL(ret);
	ZVAL_TRUE(ret);

	return ret;
}

static zval *phql_ret_insert_statement(zval *Q, zval *F, zval *V)
{
	zval *ret;

	MAKE_STD_ZVAL(ret);
	array_init_size(ret, 4);

	add_assoc_long(ret, phalcon_interned_type, PHQL_T_INSERT);
	add_assoc_zval(ret, phalcon_interned_qualifiedName, Q);
	if (F != NULL) {
		add_assoc_zval(ret, phalcon_interned_fields, F);
	}
	add_assoc_zval(ret, phalcon_interned_values, V);

	return ret;
}

static zval *phql_ret_insert_statement2(zval *ret, zval *F, zval *V)
{
	zval *key1, *key2, *rows, *values;

	MAKE_STD_ZVAL(key1);
	ZVAL_STRING(key1, phalcon_interned_rows, 1);

	MAKE_STD_ZVAL(rows);
	if (!phalcon_array_isset_fetch(&rows, ret, key1)) {
		array_init_size(rows, 1);		

		MAKE_STD_ZVAL(key2);
		ZVAL_STRING(key2, phalcon_interned_values, 1);

		MAKE_STD_ZVAL(values);
		if (phalcon_array_isset_fetch(&values, ret, key2)) {
			Z_ADDREF_P(values);
			add_next_index_zval(rows, values);	
		}
	}

	add_next_index_zval(rows, V);
	Z_ADDREF_P(rows);
	add_assoc_zval(ret, phalcon_interned_rows, rows);

	return ret;
}

static zval *phql_ret_update_statement(zval *U, zval *W, zval *L)
{
	zval *ret;

	MAKE_STD_ZVAL(ret);
	array_init(ret);

	add_assoc_long(ret, phalcon_interned_type, PHQL_T_UPDATE);
	add_assoc_zval(ret, phalcon_interned_update, U);
	if (W != NULL) {
		add_assoc_zval(ret, phalcon_interned_where, W);
	}
	if (L != NULL) {
		add_assoc_zval(ret, phalcon_interned_limit, L);
	}

	return ret;
}

static zval *phql_ret_update_clause(zval *tables, zval *values)
{
	zval *ret;

	MAKE_STD_ZVAL(ret);
	array_init_size(ret, 2);
	add_assoc_zval(ret, phalcon_interned_tables, tables);
	add_assoc_zval(ret, phalcon_interned_values, values);

	return ret;
}

static zval *phql_ret_update_item(zval *column, zval *expr)
{
	zval *ret;

	MAKE_STD_ZVAL(ret);
	array_init_size(ret, 2);
	add_assoc_zval(ret, phalcon_interned_column, column);
	add_assoc_zval(ret, phalcon_interned_expr, expr);

	return ret;
}

static zval *phql_ret_delete_statement(zval *D, zval *W, zval *L)
{
	zval *ret;

	MAKE_STD_ZVAL(ret);
	array_init(ret);

	add_assoc_long(ret, phalcon_interned_type, PHQL_T_DELETE);
	add_assoc_zval(ret, phalcon_interned_delete, D);
	if (W != NULL) {
		add_assoc_zval(ret, phalcon_interned_where, W);
	}
	if (L != NULL) {
		add_assoc_zval(ret, phalcon_interned_limit, L);
	}

	return ret;
}

static zval *phql_ret_delete_clause(zval *tables)
{
	zval *ret;

	MAKE_STD_ZVAL(ret);
	array_init_size(ret, 1);
	add_assoc_zval(ret, phalcon_interned_tables, tables);

	return ret;
}

static zval *phql_ret_zval_list(zval *list_left, zval *right_list)
{
	zval *ret;
	HashTable *list;

	MAKE_STD_ZVAL(ret);
	array_init(ret);

	list = Z_ARRVAL_P(list_left);
	if (zend_hash_index_exists(list, 0)) {
		HashPosition pos;
		zval **item;

		for (
			zend_hash_internal_pointer_reset_ex(list, &pos);
			zend_hash_get_current_data_ex(list, (void**)&item, &pos) != FAILURE;
			zend_hash_move_forward_ex(list, &pos)
		) {
			Z_ADDREF_PP(item);
			add_next_index_zval(ret, *item);
		}

		zval_ptr_dtor(&list_left);
	} else {
		add_next_index_zval(ret, list_left);
	}

	if (right_list) {
		add_next_index_zval(ret, right_list);
	}

	return ret;
}

static zval *phql_ret_column_item(int type, zval *column, phql_parser_token *identifier_column, phql_parser_token *alias)
{
	zval *ret;

	MAKE_STD_ZVAL(ret);
	array_init_size(ret, 4);
	add_assoc_long(ret, phalcon_interned_type, type);
	if (column) {
		add_assoc_zval(ret, phalcon_interned_column, column);
	}
	if (identifier_column) {
		add_assoc_stringl(ret, phalcon_interned_column, identifier_column->token, identifier_column->token_len, 0);
		efree(identifier_column);
	}
	if (alias) {
		add_assoc_stringl(ret, phalcon_interned_alias, alias->token, alias->token_len, 0);
		efree(alias);
	}

	return ret;
}

static zval *phql_ret_assoc_name(zval *qualified_name, phql_parser_token *alias)
{
	zval *ret;

	MAKE_STD_ZVAL(ret);
	array_init_size(ret, 2);
	add_assoc_zval(ret, phalcon_interned_qualifiedName, qualified_name);
	if (alias) {
		add_assoc_stringl(ret, phalcon_interned_alias, alias->token, alias->token_len, 0);
		efree(alias);
	}

	return ret;
}

static zval *phql_ret_join_type(int type)
{
	zval *ret;

	MAKE_STD_ZVAL(ret);
	ZVAL_LONG(ret, type);

	return ret;
}

static zval *phql_ret_join_item(zval *type, zval *qualified, zval *alias, zval *conditions)
{
	zval *ret;

	MAKE_STD_ZVAL(ret);
	array_init_size(ret, 4);
	add_assoc_zval(ret, phalcon_interned_type, type);

	if (qualified) {
		add_assoc_zval(ret, phalcon_interned_qualified, qualified);
	}

	if (alias) {
		add_assoc_zval(ret, phalcon_interned_alias, alias);
	}

	if (conditions) {
		add_assoc_zval(ret, phalcon_interned_conditions, conditions);
	}

	return ret;
}

static zval *phql_ret_expr(int type, zval *left, zval *right)
{
	zval *ret;

	MAKE_STD_ZVAL(ret);
	array_init_size(ret, 2);
	add_assoc_long(ret, phalcon_interned_type, type);
	if (left) {
		add_assoc_zval(ret, phalcon_interned_left, left);
	}
	if (right) {
		add_assoc_zval(ret, phalcon_interned_right, right);
	}

	return ret;
}

static zval *phql_ret_func_call(phql_parser_token *name, zval *arguments, zval *distinct)
{
	zval *ret;

	MAKE_STD_ZVAL(ret);
	array_init_size(ret, 4);
	add_assoc_long(ret, phalcon_interned_type, PHQL_T_FCALL);
	add_assoc_stringl(ret, phalcon_interned_name, name->token, name->token_len, 0);
	efree(name);

	if (arguments) {
		add_assoc_zval(ret, phalcon_interned_arguments, arguments);
	}
	
	if (distinct) {
		add_assoc_zval(ret, phalcon_interned_distinct, distinct);
	}

	return ret;
}

/* #line 497 "parser.c" */
/**************** End of %include directives **********************************/
/* These constants specify the various numeric values for terminal symbols
** in a format understandable to "makeheaders".  This section is blank unless
** "lemon" is run with the "-m" command-line option.
***************** Begin makeheaders token definitions *************************/
/**************** End makeheaders token definitions ***************************/

/* The next sections is a series of control #defines.
** various aspects of the generated parser.
**    YYCODETYPE         is the data type used to store the integer codes
**                       that represent terminal and non-terminal symbols.
**                       "unsigned char" is used if there are fewer than
**                       256 symbols.  Larger types otherwise.
**    YYNOCODE           is a number of type YYCODETYPE that is not used for
**                       any terminal or nonterminal symbol.
**    YYFALLBACK         If defined, this indicates that one or more tokens
**                       (also known as: "terminal symbols") have fall-back
**                       values which should be used if the original symbol
**                       would not parse.  This permits keywords to sometimes
**                       be used as identifiers, for example.
**    YYACTIONTYPE       is the data type used for "action codes" - numbers
**                       that indicate what to do in response to the next
**                       token.
**    phql_TOKENTYPE     is the data type used for minor type for terminal
**                       symbols.  Background: A "minor type" is a semantic
**                       value associated with a terminal or non-terminal
**                       symbols.  For example, for an "ID" terminal symbol,
**                       the minor type might be the name of the identifier.
**                       Each non-terminal can have a different minor type.
**                       Terminal symbols all have the same minor type, though.
**                       This macros defines the minor type for terminal 
**                       symbols.
**    YYMINORTYPE        is the data type used for all minor types.
**                       This is typically a union of many types, one of
**                       which is phql_TOKENTYPE.  The entry in the union
**                       for terminal symbols is called "yy0".
**    YYSTACKDEPTH       is the maximum depth of the parser's stack.  If
**                       zero the stack is dynamically sized using realloc()
**    phql_ARG_SDECL     A static variable declaration for the %extra_argument
**    phql_ARG_PDECL     A parameter declaration for the %extra_argument
**    phql_ARG_STORE     Code to store %extra_argument into yypParser
**    phql_ARG_FETCH     Code to extract %extra_argument from yypParser
**    YYERRORSYMBOL      is the code number of the error symbol.  If not
**                       defined, then do no error processing.
**    YYNSTATE           the combined number of states.
**    YYNRULE            the number of rules in the grammar
**    YY_MAX_SHIFT       Maximum value for shift actions
**    YY_MIN_SHIFTREDUCE Minimum value for shift-reduce actions
**    YY_MAX_SHIFTREDUCE Maximum value for shift-reduce actions
**    YY_MIN_REDUCE      Maximum value for reduce actions
**    YY_ERROR_ACTION    The yy_action[] code for syntax error
**    YY_ACCEPT_ACTION   The yy_action[] code for accept
**    YY_NO_ACTION       The yy_action[] code for no-op
*/
#ifndef INTERFACE
# define INTERFACE 1
#endif
/************* Begin control #defines *****************************************/
#define YYCODETYPE unsigned char
#define YYNOCODE 140
#define YYACTIONTYPE unsigned short int
#define phql_TOKENTYPE phql_parser_token*
typedef union {
  int yyinit;
  phql_TOKENTYPE yy0;
  zval* yy92;
} YYMINORTYPE;
#ifndef YYSTACKDEPTH
#define YYSTACKDEPTH 100
#endif
#define phql_ARG_SDECL phql_parser_status *status;
#define phql_ARG_PDECL ,phql_parser_status *status
#define phql_ARG_FETCH phql_parser_status *status = yypParser->status
#define phql_ARG_STORE yypParser->status = status
#define YYNSTATE             204
#define YYNRULE              163
#define YY_MAX_SHIFT         203
#define YY_MIN_SHIFTREDUCE   301
#define YY_MAX_SHIFTREDUCE   463
#define YY_MIN_REDUCE        464
#define YY_MAX_REDUCE        626
#define YY_ERROR_ACTION      627
#define YY_ACCEPT_ACTION     628
#define YY_NO_ACTION         629
/************* End control #defines *******************************************/

/* Define the yytestcase() macro to be a no-op if is not already defined
** otherwise.
**
** Applications can choose to define yytestcase() in the %include section
** to a macro that can assist in verifying code coverage.  For production
** code the yytestcase() macro should be turned off.  But it is useful
** for testing.
*/
#ifndef yytestcase
# define yytestcase(X)
#endif


/* Next are the tables used to determine what action to take based on the
** current state and lookahead token.  These tables are used to implement
** functions that take a state number and lookahead value and return an
** action integer.  
**
** Suppose the action integer is N.  Then the action is determined as
** follows
**
**   0 <= N <= YY_MAX_SHIFT             Shift N.  That is, push the lookahead
**                                      token onto the stack and goto state N.
**
**   N between YY_MIN_SHIFTREDUCE       Shift to an arbitrary state then
**     and YY_MAX_SHIFTREDUCE           reduce by rule N-YY_MIN_SHIFTREDUCE.
**
**   N between YY_MIN_REDUCE            Reduce by rule N-YY_MIN_REDUCE
**     and YY_MAX_REDUCE

**   N == YY_ERROR_ACTION               A syntax error has occurred.
**
**   N == YY_ACCEPT_ACTION              The parser accepts its input.
**
**   N == YY_NO_ACTION                  No such action.  Denotes unused
**                                      slots in the yy_action[] table.
**
** The action table is constructed as a single large table named yy_action[].
** Given state S and lookahead X, the action is computed as
**
**      yy_action[ yy_shift_ofst[S] + X ]
**
** If the index value yy_shift_ofst[S]+X is out of range or if the value
** yy_lookahead[yy_shift_ofst[S]+X] is not equal to X or if yy_shift_ofst[S]
** is equal to YY_SHIFT_USE_DFLT, it means that the action is not in the table
** and that yy_default[S] should be used instead.  
**
** The formula above is for computing the action when the lookahead is
** a terminal symbol.  If the lookahead is a non-terminal (as occurs after
** a reduce action) then the yy_reduce_ofst[] array is used in place of
** the yy_shift_ofst[] array and YY_REDUCE_USE_DFLT is used in place of
** YY_SHIFT_USE_DFLT.
**
** The following are the tables generated in this section:
**
**  yy_action[]        A single table containing all actions.
**  yy_lookahead[]     A table containing the lookahead for each entry in
**                     yy_action.  Used to detect hash collisions.
**  yy_shift_ofst[]    For each state, the offset into yy_action for
**                     shifting terminals.
**  yy_reduce_ofst[]   For each state, the offset into yy_action for
**                     shifting non-terminals after a reduce.
**  yy_default[]       Default action for each state.
**
*********** Begin parsing tables **********************************************/
#define YY_ACTTAB_COUNT (857)
static const YYACTIONTYPE yy_action[] = {
 /*     0 */    34,   33,   49,   48,   47,   46,   45,   38,   44,   43,
 /*    10 */    42,   41,   40,   39,   54,   53,   37,   35,   52,   51,
 /*    20 */    50,   56,   57,   55,   58,   60,  151,  199,  135,   34,
 /*    30 */    33,   49,   48,   47,   46,   45,   38,   44,   43,   42,
 /*    40 */    41,   40,   39,   54,   53,   37,   35,   52,   51,   50,
 /*    50 */    56,   57,   55,   58,   60,  151,  199,  135,  386,  387,
 /*    60 */   388,  389,   13,  308,  317,  149,  169,   61,  324,  325,
 /*    70 */    71,  323,  325,   71,   28,  328,   24,  178,   34,   33,
 /*    80 */    49,   48,   47,   46,   45,   38,   44,   43,   42,   41,
 /*    90 */    40,   39,   54,   53,   37,   35,   52,   51,   50,   56,
 /*   100 */    57,   55,   58,   60,  151,  199,  135,   49,   48,   47,
 /*   110 */    46,   45,   38,   44,   43,   42,   41,   40,   39,   54,
 /*   120 */    53,   37,   35,   52,   51,   50,   56,   57,   55,   58,
 /*   130 */    60,  151,  199,  135,  349,  369,  370,   34,   33,   49,
 /*   140 */    48,   47,   46,   45,   38,   44,   43,   42,   41,   40,
 /*   150 */    39,   54,   53,   37,   35,   52,   51,   50,   56,   57,
 /*   160 */    55,   58,   60,  151,  199,  135,   54,   53,   37,   35,
 /*   170 */    52,   51,   50,   56,   57,   55,   58,   60,  151,  199,
 /*   180 */   135,  156,  353,  155,  447,   78,   34,   33,   49,   48,
 /*   190 */    47,   46,   45,   38,   44,   43,   42,   41,   40,   39,
 /*   200 */    54,   53,   37,   35,   52,   51,   50,   56,   57,   55,
 /*   210 */    58,   60,  151,  199,  135,   34,   33,   49,   48,   47,
 /*   220 */    46,   45,   38,   44,   43,   42,   41,   40,   39,   54,
 /*   230 */    53,   37,   35,   52,   51,   50,   56,   57,   55,   58,
 /*   240 */    60,  151,  199,  135,   37,   35,   52,   51,   50,   56,
 /*   250 */    57,   55,   58,   60,  151,  199,  135,  426,   28,  382,
 /*   260 */    24,   27,   52,   51,   50,   56,   57,   55,   58,   60,
 /*   270 */   151,  199,  135,   56,   57,   55,   58,   60,  151,  199,
 /*   280 */   135,   58,   60,  151,  199,  135,  188,  380,   34,   33,
 /*   290 */    49,   48,   47,   46,   45,   38,   44,   43,   42,   41,
 /*   300 */    40,   39,   54,   53,   37,   35,   52,   51,   50,   56,
 /*   310 */    57,   55,   58,   60,  151,  199,  135,  177,  338,  176,
 /*   320 */   148,  379,  147,  146,  139,  191,  348,   34,   33,   49,
 /*   330 */    48,   47,   46,   45,   38,   44,   43,   42,   41,   40,
 /*   340 */    39,   54,   53,   37,   35,   52,   51,   50,   56,   57,
 /*   350 */    55,   58,   60,  151,  199,  135,  440,   36,   21,   59,
 /*   360 */   315,   94,   26,   25,  337,   78,    5,  173,  163,  134,
 /*   370 */   150,  320,  357,  462,  313,   93,  145,  145,  448,  157,
 /*   380 */     7,  196,  367,  440,  145,    6,   59,  361,  361,   26,
 /*   390 */    25,  432,  448,  352,  155,  361,  134,  195,  449,  450,
 /*   400 */   456,  457,  194,  192,  189,  432,   29,    7,  151,  199,
 /*   410 */   135,  141,  453,  451,  452,  454,  455,  458,  459,  162,
 /*   420 */   164,   77,  361,   92,  152,  449,  450,  456,  457,  194,
 /*   430 */   192,  189,  361,   29,  168,  427,  104,  448,   92,  453,
 /*   440 */   451,  452,  454,  455,  458,  459,  104,  442,  108,  314,
 /*   450 */   432,   95,   59,  448,  193,   26,   25,  138,   92,  345,
 /*   460 */    86,  142,  133,  448,  319,  448,  432,  439,  448,  145,
 /*   470 */    68,  185,   72,    7,  160,  170,  432,  439,  432,   59,
 /*   480 */   361,  432,   26,   25,  100,   78,  104,   76,  428,  134,
 /*   490 */   356,  449,  450,  456,  457,  194,  192,  189,  102,   29,
 /*   500 */     7,  448,  359,  448,  172,  453,  451,  452,  454,  455,
 /*   510 */   458,  459,  309,  310,  432,  448,  432,  438,  449,  450,
 /*   520 */   456,  457,  194,  192,  189,  179,   29,   22,  432,  144,
 /*   530 */   179,   79,  453,  451,  452,  454,  455,  458,  459,   85,
 /*   540 */    85,  350,   44,   43,   42,   41,   40,   39,   54,   53,
 /*   550 */    37,   35,   52,   51,   50,   56,   57,   55,   58,   60,
 /*   560 */   151,  199,  135,   59,   81,  307,   26,   25,  347,   61,
 /*   570 */   324,  325,   71,  134,  182,  434,   19,   65,  182,  182,
 /*   580 */   335,   74,    3,  174,    7,  177,  338,  176,  148,    4,
 /*   590 */   147,  146,   78,  343,  628,  203,  302,  202,  304,  305,
 /*   600 */    92,   32,  449,  450,  456,  457,  194,  192,  189,  161,
 /*   610 */    29,  165,   64,   73,  154,   92,  453,  451,  452,  454,
 /*   620 */   455,  458,  459,  326,  171,   83,   82,  108,  104,  198,
 /*   630 */    19,  104,   80,   75,   15,   15,  140,   87,  345,   14,
 /*   640 */   197,   20,  159,   23,  448,  448,  101,  342,  448,  312,
 /*   650 */    93,  418,  417,  143,  101,  108,  185,  432,  432,  439,
 /*   660 */   186,  432,  439,  448,  153,   99,  345,  448,  166,  374,
 /*   670 */    94,  448,  448,  103,   88,  108,  432,  373,  306,  183,
 /*   680 */   432,  184,  448,  354,  432,  432,  344,  448,   89,   90,
 /*   690 */   448,  366,  448,  105,  167,  432,  448,   91,  448,   96,
 /*   700 */   432,   62,   66,  432,   97,  432,   16,   98,  106,  432,
 /*   710 */   448,  432,  333,  109,  107,  175,  448,   67,  124,  448,
 /*   720 */    63,  448,  200,  432,  448,  448,    8,   69,  125,  432,
 /*   730 */   448,  448,  432,  158,  432,  448,  110,  432,  432,  448,
 /*   740 */   116,    9,   19,  432,  432,  448,  117,    1,  432,  135,
 /*   750 */   421,  180,  432,  448,  118,  420,  119,  448,  432,  341,
 /*   760 */    17,  120,  121,  448,  111,  112,  432,   10,  113,  419,
 /*   770 */   432,  448,  114,  448,  316,  431,  432,   84,  448,  448,
 /*   780 */   115,  448,  448,  126,  432,  448,  432,  127,  128,  448,
 /*   790 */   122,  432,  432,  358,  432,  432,  336,  448,  432,  123,
 /*   800 */   448,  334,  432,  332,  448,  448,  129,  448,  130,  331,
 /*   810 */   432,  131,  330,  432,  327,  136,  448,  432,  432,  462,
 /*   820 */   432,  460,  137,  448,  181,  448,  132,   15,  448,  432,
 /*   830 */   433,  187,  448,  190,  425,  424,  432,   30,  432,  448,
 /*   840 */    31,  432,  422,  448,  443,  432,   70,   18,   11,    2,
 /*   850 */   384,   12,  432,  201,  464,  466,  432,
};
static const YYCODETYPE yy_lookahead[] = {
 /*     0 */     1,    2,    3,    4,    5,    6,    7,    8,    9,   10,
 /*    10 */    11,   12,   13,   14,   15,   16,   17,   18,   19,   20,
 /*    20 */    21,   22,   23,   24,   25,   26,   27,   28,   29,    1,
 /*    30 */     2,    3,    4,    5,    6,    7,    8,    9,   10,   11,
 /*    40 */    12,   13,   14,   15,   16,   17,   18,   19,   20,   21,
 /*    50 */    22,   23,   24,   25,   26,   27,   28,   29,   65,   66,
 /*    60 */    67,   68,  104,  105,   36,   36,   38,  109,  110,  111,
 /*    70 */   112,  110,  111,  112,   75,   36,   77,   38,    1,    2,
 /*    80 */     3,    4,    5,    6,    7,    8,    9,   10,   11,   12,
 /*    90 */    13,   14,   15,   16,   17,   18,   19,   20,   21,   22,
 /*   100 */    23,   24,   25,   26,   27,   28,   29,    3,    4,    5,
 /*   110 */     6,    7,    8,    9,   10,   11,   12,   13,   14,   15,
 /*   120 */    16,   17,   18,   19,   20,   21,   22,   23,   24,   25,
 /*   130 */    26,   27,   28,   29,   36,   58,   59,    1,    2,    3,
 /*   140 */     4,    5,    6,    7,    8,    9,   10,   11,   12,   13,
 /*   150 */    14,   15,   16,   17,   18,   19,   20,   21,   22,   23,
 /*   160 */    24,   25,   26,   27,   28,   29,   15,   16,   17,   18,
 /*   170 */    19,   20,   21,   22,   23,   24,   25,   26,   27,   28,
 /*   180 */    29,  122,  123,  124,   48,   32,    1,    2,    3,    4,
 /*   190 */     5,    6,    7,    8,    9,   10,   11,   12,   13,   14,
 /*   200 */    15,   16,   17,   18,   19,   20,   21,   22,   23,   24,
 /*   210 */    25,   26,   27,   28,   29,    1,    2,    3,    4,    5,
 /*   220 */     6,    7,    8,    9,   10,   11,   12,   13,   14,   15,
 /*   230 */    16,   17,   18,   19,   20,   21,   22,   23,   24,   25,
 /*   240 */    26,   27,   28,   29,   17,   18,   19,   20,   21,   22,
 /*   250 */    23,   24,   25,   26,   27,   28,   29,   74,   75,  131,
 /*   260 */    77,   76,   19,   20,   21,   22,   23,   24,   25,   26,
 /*   270 */    27,   28,   29,   22,   23,   24,   25,   26,   27,   28,
 /*   280 */    29,   25,   26,   27,   28,   29,   72,  131,    1,    2,
 /*   290 */     3,    4,    5,    6,    7,    8,    9,   10,   11,   12,
 /*   300 */    13,   14,   15,   16,   17,   18,   19,   20,   21,   22,
 /*   310 */    23,   24,   25,   26,   27,   28,   29,   39,   40,   41,
 /*   320 */    42,  131,   44,   45,  117,   38,  119,    1,    2,    3,
 /*   330 */     4,    5,    6,    7,    8,    9,   10,   11,   12,   13,
 /*   340 */    14,   15,   16,   17,   18,   19,   20,   21,   22,   23,
 /*   350 */    24,   25,   26,   27,   28,   29,   23,   17,   18,   26,
 /*   360 */    23,  107,   29,   30,   40,   32,  103,   43,   28,   36,
 /*   370 */   102,  108,  108,   36,  106,  107,  113,  113,  124,  108,
 /*   380 */    47,  127,  128,   23,  113,  101,   26,  124,  124,   29,
 /*   390 */    30,  137,  124,  123,  124,  124,   36,   29,   65,   66,
 /*   400 */    67,   68,   69,   70,   71,  137,   73,   47,   27,   28,
 /*   410 */    29,  113,   79,   80,   81,   82,   83,   84,   85,   90,
 /*   420 */   107,  113,  124,   94,  131,   65,   66,   67,   68,   69,
 /*   430 */    70,   71,  124,   73,   90,  134,  107,  124,   94,   79,
 /*   440 */    80,   81,   82,   83,   84,   85,  107,   79,  107,   23,
 /*   450 */   137,  107,   26,  124,   90,   29,   30,  116,   94,  118,
 /*   460 */   114,  132,   36,  124,  108,  124,  137,  138,  124,  113,
 /*   470 */    47,  132,   62,   47,   51,  136,  137,  138,  137,   26,
 /*   480 */   124,  137,   29,   30,  107,   32,  107,  133,  134,   36,
 /*   490 */   121,   65,   66,   67,   68,   69,   70,   71,  107,   73,
 /*   500 */    47,  124,   36,  124,   38,   79,   80,   81,   82,   83,
 /*   510 */    84,   85,   34,   35,  137,  124,  137,  138,   65,   66,
 /*   520 */    67,   68,   69,   70,   71,   37,   73,   55,  137,   37,
 /*   530 */    37,   95,   79,   80,   81,   82,   83,   84,   85,   47,
 /*   540 */    47,  121,    9,   10,   11,   12,   13,   14,   15,   16,
 /*   550 */    17,   18,   19,   20,   21,   22,   23,   24,   25,   26,
 /*   560 */    27,   28,   29,   26,   95,  105,   29,   30,  119,  109,
 /*   570 */   110,  111,  112,   36,   86,   34,   31,   31,   86,   86,
 /*   580 */    40,   31,  135,   43,   47,   39,   40,   41,   42,  135,
 /*   590 */    44,   45,   32,   48,   88,   89,   90,   91,   92,   93,
 /*   600 */    94,   46,   65,   66,   67,   68,   69,   70,   71,   49,
 /*   610 */    73,   90,   52,   63,   54,   94,   79,   80,   81,   82,
 /*   620 */    83,   84,   85,  115,   78,   31,  120,  107,  107,   64,
 /*   630 */    31,  107,  126,   62,   31,   31,  116,   99,  118,   31,
 /*   640 */    56,   33,   48,   61,  124,  124,  107,   48,  124,  106,
 /*   650 */   107,   48,   48,  132,  107,  107,  132,  137,  137,  138,
 /*   660 */   136,  137,  138,  124,  116,  107,  118,  124,  129,  130,
 /*   670 */   107,  124,  124,  107,   98,  107,  137,  130,  100,  107,
 /*   680 */   137,  107,  124,  125,  137,  137,  118,  124,   97,   96,
 /*   690 */   124,  128,  124,  107,   60,  137,  124,   95,  124,  107,
 /*   700 */   137,   33,  107,  137,  107,  137,    3,  107,  107,  137,
 /*   710 */   124,  137,   40,  107,  107,   43,  124,   31,  107,  124,
 /*   720 */    53,  124,  107,  137,  124,  124,   47,   50,  107,  137,
 /*   730 */   124,  124,  137,   51,  137,  124,  107,  137,  137,  124,
 /*   740 */   107,   47,   31,  137,  137,  124,  107,   47,  137,   29,
 /*   750 */    48,   37,  137,  124,  107,   48,  107,  124,  137,   48,
 /*   760 */    31,  107,  107,  124,  107,  107,  137,   57,  107,   48,
 /*   770 */   137,  124,  107,  124,   36,   48,  137,   47,  124,  124,
 /*   780 */   107,  124,  124,  107,  137,  124,  137,  107,  107,  124,
 /*   790 */   107,  137,  137,   36,  137,  137,   40,  124,  137,  107,
 /*   800 */   124,   40,  137,   40,  124,  124,  107,  124,  107,   40,
 /*   810 */   137,  107,   40,  137,   36,  107,  124,  137,  137,   36,
 /*   820 */   137,   36,  107,  124,   36,  124,  107,   31,  124,  137,
 /*   830 */    48,   36,  124,   36,   48,   48,  137,   47,  137,  124,
 /*   840 */    47,  137,   48,  124,   79,  137,   47,   31,   57,   47,
 /*   850 */    52,   47,  137,   31,    0,  139,  137,
};
#define YY_SHIFT_USE_DFLT (-8)
#define YY_SHIFT_COUNT (203)
#define YY_SHIFT_MIN   (-7)
#define YY_SHIFT_MAX   (854)
static const short yy_shift_ofst[] = {
 /*     0 */   560,  333,  333,  360,  360,  546,  426,  453,  537,  537,
 /*    10 */   537,  537,  537,  278,  426,  360,  537,  537,  537,  537,
 /*    20 */    29,  537,  537,  537,  537,  537,  537,  537,  537,  537,
 /*    30 */   537,  537,  537,  537,  537,  537,  537,  537,  537,  537,
 /*    40 */   537,  537,  537,  537,  537,  537,  537,  537,  537,  537,
 /*    50 */   537,  537,  537,  537,  537,  537,  537,  537,  537,  537,
 /*    60 */   537,  278,   29,   29,   29,   29,   -1,   29,   98,   29,
 /*    70 */   153,   29,   -7,   -7,   -7,   -7,  183,   39,  478,  410,
 /*    80 */   472,  410,  472,   98,  541,  541,  555,  565,  571,  584,
 /*    90 */   582,  634,  472,   28,   77,  136,  185,  214,  287,  326,
 /*   100 */   326,  326,  326,  326,  326,  326,  326,  326,  326,  104,
 /*   110 */   533,  533,  533,  533,  533,  533,  151,  151,  151,  151,
 /*   120 */   151,  151,  227,  227,  243,  243,  251,  251,  251,  256,
 /*   130 */   256,  256,  381,  492,  493,  340,  381,  381,  545,  594,
 /*   140 */   599,  423,  603,  604,  337,  466,  324,  540,  672,  488,
 /*   150 */   608,  368,  550,  711,  668,  703,  686,  667,  679,  682,
 /*   160 */   694,  677,  702,  700,  720,  707,  729,  710,  721,  738,
 /*   170 */   727,  730,  757,  756,  761,  763,  769,  772,  778,  783,
 /*   180 */   785,  714,  788,  720,  720,  796,  782,  786,  795,  790,
 /*   190 */   787,  797,  793,  794,  799,  765,  816,  791,  798,  802,
 /*   200 */   720,  804,  822,  854,
};
#define YY_REDUCE_USE_DFLT (-43)
#define YY_REDUCE_COUNT (92)
#define YY_REDUCE_MIN   (-42)
#define YY_REDUCE_MAX   (719)
static const short yy_reduce_ofst[] = {
 /*     0 */   506,  329,  521,  339,  524,  -42,  268,  344,  341,  520,
 /*    10 */   539,  254,  548,  460,  543,  379,  558,  547,  563,  568,
 /*    20 */   263,  313,  377,  391,  566,  572,  574,  586,  592,  595,
 /*    30 */   597,  600,  601,  606,  607,  611,  615,  621,  629,  633,
 /*    40 */   639,  647,  649,  654,  655,  657,  658,  661,  665,  673,
 /*    50 */   676,  680,  681,  683,  692,  699,  701,  704,  708,  715,
 /*    60 */   719,  -39,  264,   59,  271,  356,  354,  270,  207,  298,
 /*    70 */   364,  308,  128,  156,  190,  293,  301,  346,  284,  369,
 /*    80 */   436,  420,  469,  449,  447,  454,  508,  578,  538,  576,
 /*    90 */   591,  593,  602,
};
static const YYACTIONTYPE yy_default[] = {
 /*     0 */   627,  627,  627,  600,  600,  485,  627,  627,  627,  627,
 /*    10 */   627,  627,  627,  485,  627,  627,  627,  627,  627,  627,
 /*    20 */   627,  627,  627,  627,  627,  627,  627,  627,  627,  627,
 /*    30 */   627,  627,  627,  627,  627,  627,  627,  627,  627,  627,
 /*    40 */   627,  627,  627,  627,  627,  627,  627,  627,  627,  627,
 /*    50 */   627,  627,  627,  627,  627,  627,  627,  627,  627,  627,
 /*    60 */   627,  484,  627,  627,  627,  627,  627,  627,  627,  627,
 /*    70 */   627,  627,  627,  627,  627,  627,  627,  492,  474,  546,
 /*    80 */   526,  546,  526,  627,  598,  598,  503,  548,  544,  528,
 /*    90 */   540,  535,  526,  481,  531,  627,  627,  627,  627,  518,
 /*   100 */   525,  538,  539,  593,  604,  592,  502,  586,  509,  607,
 /*   110 */   575,  568,  567,  566,  565,  564,  574,  573,  572,  571,
 /*   120 */   570,  569,  560,  559,  578,  576,  563,  562,  561,  558,
 /*   130 */   557,  556,  554,  626,  626,  627,  555,  553,  627,  627,
 /*   140 */   627,  627,  627,  627,  627,  523,  627,  627,  627,  626,
 /*   150 */   627,  627,  541,  627,  627,  627,  514,  627,  627,  627,
 /*   160 */   627,  627,  627,  627,  579,  627,  534,  627,  627,  627,
 /*   170 */   627,  627,  627,  627,  627,  627,  627,  627,  627,  627,
 /*   180 */   627,  624,  627,  609,  608,  599,  627,  627,  627,  627,
 /*   190 */   627,  627,  627,  627,  627,  627,  527,  627,  627,  627,
 /*   200 */   577,  627,  466,  627,
};
/********** End of lemon-generated parsing tables *****************************/

/* The next table maps tokens (terminal symbols) into fallback tokens.  
** If a construct like the following:
** 
**      %fallback ID X Y Z.
**
** appears in the grammar, then ID becomes a fallback token for X, Y,
** and Z.  Whenever one of the tokens X, Y, or Z is input to the parser
** but it does not parse, the type of the token is changed to ID and
** the parse is retried before an error is thrown.
**
** This feature can be used, for example, to cause some keywords in a language
** to revert to identifiers if they keyword does not apply in the context where
** it appears.
*/
#ifdef YYFALLBACK
static const YYCODETYPE yyFallback[] = {
};
#endif /* YYFALLBACK */

/* The following structure represents a single element of the
** parser's stack.  Information stored includes:
**
**   +  The state number for the parser at this level of the stack.
**
**   +  The value of the token stored at this level of the stack.
**      (In other words, the "major" token.)
**
**   +  The semantic value stored at this level of the stack.  This is
**      the information used by the action routines in the grammar.
**      It is sometimes called the "minor" token.
**
** After the "shift" half of a SHIFTREDUCE action, the stateno field
** actually contains the reduce action for the second half of the
** SHIFTREDUCE.
*/
struct yyStackEntry {
  YYACTIONTYPE stateno;  /* The state-number, or reduce action in SHIFTREDUCE */
  YYCODETYPE major;      /* The major token value.  This is the code
                         ** number for the token at this stack level */
  YYMINORTYPE minor;     /* The user-supplied minor token value.  This
                         ** is the value of the token  */
};
typedef struct yyStackEntry yyStackEntry;

/* The state of the parser is completely contained in an instance of
** the following structure */
struct yyParser {
  int yyidx;                    /* Index of top element in stack */
#ifdef YYTRACKMAXSTACKDEPTH
  int yyidxMax;                 /* Maximum value of yyidx */
#endif
#ifndef YYNOERRORRECOVERY
  int yyerrcnt;                 /* Shifts left before out of the error */
#endif
  phql_ARG_SDECL                /* A place to hold %extra_argument */
#if YYSTACKDEPTH<=0
  int yystksz;                  /* Current side of the stack */
  yyStackEntry *yystack;        /* The parser's stack */
#else
  yyStackEntry yystack[YYSTACKDEPTH];  /* The parser's stack */
#endif
};
typedef struct yyParser yyParser;

#ifndef NDEBUG
#include <stdio.h>
static FILE *yyTraceFILE = 0;
static char *yyTracePrompt = 0;
#endif /* NDEBUG */

#ifndef NDEBUG
/* 
** Turn parser tracing on by giving a stream to which to write the trace
** and a prompt to preface each trace message.  Tracing is turned off
** by making either argument NULL 
**
** Inputs:
** <ul>
** <li> A FILE* to which trace output should be written.
**      If NULL, then tracing is turned off.
** <li> A prefix string written at the beginning of every
**      line of trace output.  If NULL, then tracing is
**      turned off.
** </ul>
**
** Outputs:
** None.
*/
void phql_Trace(FILE *TraceFILE, char *zTracePrompt){
  yyTraceFILE = TraceFILE;
  yyTracePrompt = zTracePrompt;
  if( yyTraceFILE==0 ) yyTracePrompt = 0;
  else if( yyTracePrompt==0 ) yyTraceFILE = 0;
}
#endif /* NDEBUG */

#ifndef NDEBUG
/* For tracing shifts, the names of all terminals and nonterminals
** are required.  The following table supplies these names */
static const char *const yyTokenName[] = { 
  "$",             "AGAINST",       "BETWEEN",       "EQUALS",      
  "NOTEQUALS",     "LESS",          "GREATER",       "GREATEREQUAL",
  "LESSEQUAL",     "TS_MATCHES",    "TS_OR",         "TS_AND",      
  "TS_NEGATE",     "TS_CONTAINS_ANOTHER",  "TS_CONTAINS_IN",  "AND",         
  "OR",            "LIKE",          "ILIKE",         "BITWISE_AND", 
  "BITWISE_OR",    "BITWISE_XOR",   "DIVIDE",        "TIMES",       
  "MOD",           "PLUS",          "MINUS",         "IS",          
  "IN",            "NOT",           "BITWISE_NOT",   "COMMA",       
  "SELECT",        "FROM",          "DISTINCT",      "ALL",         
  "IDENTIFIER",    "DOT",           "AS",            "INNER",       
  "JOIN",          "CROSS",         "LEFT",          "OUTER",       
  "RIGHT",         "FULL",          "ON",            "PARENTHESES_OPEN",
  "PARENTHESES_CLOSE",  "INSERT",        "INTO",          "VALUES",      
  "UPDATE",        "SET",           "DELETE",        "WHERE",       
  "ORDER",         "BY",            "ASC",           "DESC",        
  "GROUP",         "HAVING",        "LIMIT",         "OFFSET",      
  "FOR",           "HINTEGER",      "INTEGER",       "NPLACEHOLDER",
  "SPLACEHOLDER",  "EXISTS",        "CAST",          "CONVERT",     
  "USING",         "CASE",          "END",           "WHEN",        
  "THEN",          "ELSE",          "FORCEINDEX",    "NULL",        
  "STRING",        "DOUBLE",        "TRUE",          "FALSE",       
  "NTPLACEHOLDER",  "STPLACEHOLDER",  "COLON",         "error",       
  "program",       "query_language",  "select_statement",  "insert_statement",
  "update_statement",  "delete_statement",  "select_clause",  "where_clause",
  "group_clause",  "having_clause",  "order_clause",  "select_limit_clause",
  "forupdate_clause",  "distinct_all",  "column_list",   "associated_name_list",
  "force_index",   "join_list_or_null",  "column_item",   "expr",        
  "associated_name",  "join_list",     "join_item",     "join_clause", 
  "join_type",     "aliased_or_qualified_name",  "join_associated_name",  "join_conditions",
  "values_list",   "field_list",    "value_item",    "field_item",  
  "update_clause",  "limit_clause",  "update_item_list",  "update_item", 
  "qualified_name",  "new_value",     "delete_clause",  "order_list",  
  "order_item",    "group_list",    "group_item",    "integer_or_placeholder",
  "argument_list",  "when_clauses",  "when_clause",   "distinct_or_null",
  "argument_list_or_null",  "function_call",  "argument_item",
};
#endif /* NDEBUG */

#ifndef NDEBUG
/* For tracing reduce actions, the names of all rules are required.
*/
static const char *const yyRuleName[] = {
 /*   0 */ "program ::= query_language",
 /*   1 */ "query_language ::= select_statement",
 /*   2 */ "query_language ::= insert_statement",
 /*   3 */ "query_language ::= update_statement",
 /*   4 */ "query_language ::= delete_statement",
 /*   5 */ "select_statement ::= select_clause where_clause group_clause having_clause order_clause select_limit_clause forupdate_clause",
 /*   6 */ "select_clause ::= SELECT distinct_all column_list FROM associated_name_list force_index join_list_or_null",
 /*   7 */ "select_clause ::= SELECT distinct_all column_list FROM associated_name_list join_list_or_null",
 /*   8 */ "distinct_all ::= DISTINCT",
 /*   9 */ "distinct_all ::= ALL",
 /*  10 */ "distinct_all ::=",
 /*  11 */ "column_list ::= column_list COMMA column_item",
 /*  12 */ "column_list ::= column_item",
 /*  13 */ "column_item ::= TIMES",
 /*  14 */ "column_item ::= IDENTIFIER DOT TIMES",
 /*  15 */ "column_item ::= expr AS IDENTIFIER",
 /*  16 */ "column_item ::= expr IDENTIFIER",
 /*  17 */ "column_item ::= expr",
 /*  18 */ "associated_name_list ::= associated_name_list COMMA associated_name",
 /*  19 */ "associated_name_list ::= associated_name",
 /*  20 */ "join_list_or_null ::= join_list",
 /*  21 */ "join_list_or_null ::=",
 /*  22 */ "join_list ::= join_list join_item",
 /*  23 */ "join_list ::= join_item",
 /*  24 */ "join_item ::= join_clause",
 /*  25 */ "join_clause ::= join_type aliased_or_qualified_name join_associated_name join_conditions",
 /*  26 */ "join_associated_name ::= AS IDENTIFIER",
 /*  27 */ "join_associated_name ::= IDENTIFIER",
 /*  28 */ "join_associated_name ::=",
 /*  29 */ "join_type ::= INNER JOIN",
 /*  30 */ "join_type ::= CROSS JOIN",
 /*  31 */ "join_type ::= LEFT OUTER JOIN",
 /*  32 */ "join_type ::= LEFT JOIN",
 /*  33 */ "join_type ::= RIGHT OUTER JOIN",
 /*  34 */ "join_type ::= RIGHT JOIN",
 /*  35 */ "join_type ::= FULL OUTER JOIN",
 /*  36 */ "join_type ::= FULL JOIN",
 /*  37 */ "join_type ::= JOIN",
 /*  38 */ "join_conditions ::= ON expr",
 /*  39 */ "join_conditions ::=",
 /*  40 */ "insert_statement ::= insert_statement COMMA PARENTHESES_OPEN values_list PARENTHESES_CLOSE",
 /*  41 */ "insert_statement ::= INSERT INTO aliased_or_qualified_name VALUES PARENTHESES_OPEN values_list PARENTHESES_CLOSE",
 /*  42 */ "insert_statement ::= INSERT INTO aliased_or_qualified_name PARENTHESES_OPEN field_list PARENTHESES_CLOSE VALUES PARENTHESES_OPEN values_list PARENTHESES_CLOSE",
 /*  43 */ "values_list ::= values_list COMMA value_item",
 /*  44 */ "values_list ::= value_item",
 /*  45 */ "value_item ::= expr",
 /*  46 */ "field_list ::= field_list COMMA field_item",
 /*  47 */ "field_list ::= field_item",
 /*  48 */ "field_item ::= IDENTIFIER",
 /*  49 */ "update_statement ::= update_clause where_clause limit_clause",
 /*  50 */ "update_clause ::= UPDATE associated_name SET update_item_list",
 /*  51 */ "update_item_list ::= update_item_list COMMA update_item",
 /*  52 */ "update_item_list ::= update_item",
 /*  53 */ "update_item ::= qualified_name EQUALS new_value",
 /*  54 */ "new_value ::= expr",
 /*  55 */ "delete_statement ::= delete_clause where_clause limit_clause",
 /*  56 */ "delete_clause ::= DELETE FROM associated_name",
 /*  57 */ "associated_name ::= aliased_or_qualified_name AS IDENTIFIER",
 /*  58 */ "associated_name ::= aliased_or_qualified_name IDENTIFIER",
 /*  59 */ "associated_name ::= aliased_or_qualified_name",
 /*  60 */ "aliased_or_qualified_name ::= qualified_name",
 /*  61 */ "where_clause ::= WHERE expr",
 /*  62 */ "where_clause ::=",
 /*  63 */ "order_clause ::= ORDER BY order_list",
 /*  64 */ "order_clause ::=",
 /*  65 */ "order_list ::= order_list COMMA order_item",
 /*  66 */ "order_list ::= order_item",
 /*  67 */ "order_item ::= expr",
 /*  68 */ "order_item ::= expr ASC",
 /*  69 */ "order_item ::= expr DESC",
 /*  70 */ "group_clause ::= GROUP BY group_list",
 /*  71 */ "group_clause ::=",
 /*  72 */ "group_list ::= group_list COMMA group_item",
 /*  73 */ "group_list ::= group_item",
 /*  74 */ "group_item ::= expr",
 /*  75 */ "having_clause ::= HAVING expr",
 /*  76 */ "having_clause ::=",
 /*  77 */ "select_limit_clause ::= LIMIT integer_or_placeholder",
 /*  78 */ "select_limit_clause ::= LIMIT integer_or_placeholder COMMA integer_or_placeholder",
 /*  79 */ "select_limit_clause ::= LIMIT integer_or_placeholder OFFSET integer_or_placeholder",
 /*  80 */ "select_limit_clause ::=",
 /*  81 */ "limit_clause ::= LIMIT integer_or_placeholder",
 /*  82 */ "limit_clause ::=",
 /*  83 */ "forupdate_clause ::= FOR UPDATE",
 /*  84 */ "forupdate_clause ::=",
 /*  85 */ "integer_or_placeholder ::= HINTEGER",
 /*  86 */ "integer_or_placeholder ::= INTEGER",
 /*  87 */ "integer_or_placeholder ::= NPLACEHOLDER",
 /*  88 */ "integer_or_placeholder ::= SPLACEHOLDER",
 /*  89 */ "expr ::= MINUS expr",
 /*  90 */ "expr ::= expr MINUS expr",
 /*  91 */ "expr ::= expr PLUS expr",
 /*  92 */ "expr ::= expr TIMES expr",
 /*  93 */ "expr ::= expr DIVIDE expr",
 /*  94 */ "expr ::= expr MOD expr",
 /*  95 */ "expr ::= expr AND expr",
 /*  96 */ "expr ::= expr OR expr",
 /*  97 */ "expr ::= expr BITWISE_AND expr",
 /*  98 */ "expr ::= expr BITWISE_OR expr",
 /*  99 */ "expr ::= expr BITWISE_XOR expr",
 /* 100 */ "expr ::= expr EQUALS expr",
 /* 101 */ "expr ::= expr NOTEQUALS expr",
 /* 102 */ "expr ::= expr LESS expr",
 /* 103 */ "expr ::= expr GREATER expr",
 /* 104 */ "expr ::= expr GREATEREQUAL expr",
 /* 105 */ "expr ::= expr TS_MATCHES expr",
 /* 106 */ "expr ::= expr TS_OR expr",
 /* 107 */ "expr ::= expr TS_AND expr",
 /* 108 */ "expr ::= expr TS_NEGATE expr",
 /* 109 */ "expr ::= expr TS_CONTAINS_ANOTHER expr",
 /* 110 */ "expr ::= expr TS_CONTAINS_IN expr",
 /* 111 */ "expr ::= expr LESSEQUAL expr",
 /* 112 */ "expr ::= expr LIKE expr",
 /* 113 */ "expr ::= expr NOT LIKE expr",
 /* 114 */ "expr ::= expr ILIKE expr",
 /* 115 */ "expr ::= expr NOT ILIKE expr",
 /* 116 */ "expr ::= expr IN PARENTHESES_OPEN argument_list PARENTHESES_CLOSE",
 /* 117 */ "expr ::= expr NOT IN PARENTHESES_OPEN argument_list PARENTHESES_CLOSE",
 /* 118 */ "expr ::= PARENTHESES_OPEN select_statement PARENTHESES_CLOSE",
 /* 119 */ "expr ::= expr IN PARENTHESES_OPEN select_statement PARENTHESES_CLOSE",
 /* 120 */ "expr ::= expr NOT IN PARENTHESES_OPEN select_statement PARENTHESES_CLOSE",
 /* 121 */ "expr ::= EXISTS PARENTHESES_OPEN select_statement PARENTHESES_CLOSE",
 /* 122 */ "expr ::= expr AGAINST expr",
 /* 123 */ "expr ::= CAST PARENTHESES_OPEN expr AS IDENTIFIER PARENTHESES_CLOSE",
 /* 124 */ "expr ::= CONVERT PARENTHESES_OPEN expr USING IDENTIFIER PARENTHESES_CLOSE",
 /* 125 */ "expr ::= CASE expr when_clauses END",
 /* 126 */ "when_clauses ::= when_clauses when_clause",
 /* 127 */ "when_clauses ::= when_clause",
 /* 128 */ "when_clause ::= WHEN expr THEN expr",
 /* 129 */ "when_clause ::= ELSE expr",
 /* 130 */ "force_index ::= FORCEINDEX PARENTHESES_OPEN distinct_or_null argument_list_or_null PARENTHESES_CLOSE",
 /* 131 */ "expr ::= function_call",
 /* 132 */ "function_call ::= IDENTIFIER PARENTHESES_OPEN distinct_or_null argument_list_or_null PARENTHESES_CLOSE",
 /* 133 */ "distinct_or_null ::= DISTINCT",
 /* 134 */ "distinct_or_null ::=",
 /* 135 */ "argument_list_or_null ::= argument_list",
 /* 136 */ "argument_list_or_null ::=",
 /* 137 */ "argument_list ::= argument_list COMMA argument_item",
 /* 138 */ "argument_list ::= argument_item",
 /* 139 */ "argument_item ::= TIMES",
 /* 140 */ "argument_item ::= expr",
 /* 141 */ "expr ::= expr IS NULL",
 /* 142 */ "expr ::= expr IS NOT NULL",
 /* 143 */ "expr ::= expr BETWEEN expr",
 /* 144 */ "expr ::= NOT expr",
 /* 145 */ "expr ::= BITWISE_NOT expr",
 /* 146 */ "expr ::= PARENTHESES_OPEN expr PARENTHESES_CLOSE",
 /* 147 */ "expr ::= qualified_name",
 /* 148 */ "expr ::= HINTEGER",
 /* 149 */ "expr ::= INTEGER",
 /* 150 */ "expr ::= STRING",
 /* 151 */ "expr ::= DOUBLE",
 /* 152 */ "expr ::= NULL",
 /* 153 */ "expr ::= TRUE",
 /* 154 */ "expr ::= FALSE",
 /* 155 */ "expr ::= NPLACEHOLDER",
 /* 156 */ "expr ::= SPLACEHOLDER",
 /* 157 */ "expr ::= NTPLACEHOLDER",
 /* 158 */ "expr ::= STPLACEHOLDER",
 /* 159 */ "qualified_name ::= IDENTIFIER COLON IDENTIFIER DOT IDENTIFIER",
 /* 160 */ "qualified_name ::= IDENTIFIER COLON IDENTIFIER",
 /* 161 */ "qualified_name ::= IDENTIFIER DOT IDENTIFIER",
 /* 162 */ "qualified_name ::= IDENTIFIER",
};
#endif /* NDEBUG */


#if YYSTACKDEPTH<=0
/*
** Try to increase the size of the parser stack.
*/
static void yyGrowStack(yyParser *p){
  int newSize;
  yyStackEntry *pNew;

  newSize = p->yystksz*2 + 100;
  pNew = realloc(p->yystack, newSize*sizeof(pNew[0]));
  if( pNew ){
    p->yystack = pNew;
    p->yystksz = newSize;
#ifndef NDEBUG
    if( yyTraceFILE ){
      fprintf(yyTraceFILE,"%sStack grows to %d entries!\n",
              yyTracePrompt, p->yystksz);
    }
#endif
  }
}
#endif

/* Datatype of the argument to the memory allocated passed as the
** second argument to phql_Alloc() below.  This can be changed by
** putting an appropriate #define in the %include section of the input
** grammar.
*/
#ifndef YYMALLOCARGTYPE
# define YYMALLOCARGTYPE size_t
#endif

/* 
** This function allocates a new parser.
** The only argument is a pointer to a function which works like
** malloc.
**
** Inputs:
** A pointer to the function used to allocate memory.
**
** Outputs:
** A pointer to a parser.  This pointer is used in subsequent calls
** to phql_ and phql_Free.
*/
void *phql_Alloc(void *(*mallocProc)(YYMALLOCARGTYPE)){
  yyParser *pParser;
  pParser = (yyParser*)(*mallocProc)( (YYMALLOCARGTYPE)sizeof(yyParser) );
  if( pParser ){
    pParser->yyidx = -1;
#ifdef YYTRACKMAXSTACKDEPTH
    pParser->yyidxMax = 0;
#endif
#if YYSTACKDEPTH<=0
    pParser->yystack = NULL;
    pParser->yystksz = 0;
    yyGrowStack(pParser);
#endif
  }
  return pParser;
}

/* The following function deletes the "minor type" or semantic value
** associated with a symbol.  The symbol can be either a terminal
** or nonterminal. "yymajor" is the symbol code, and "yypminor" is
** a pointer to the value to be deleted.  The code used to do the 
** deletions is derived from the %destructor and/or %token_destructor
** directives of the input grammar.
*/
static void yy_destructor(
  yyParser *yypParser,    /* The parser */
  YYCODETYPE yymajor,     /* Type code for object to destroy */
  YYMINORTYPE *yypminor   /* The object to be destroyed */
){
  phql_ARG_FETCH;
  switch( yymajor ){
    /* Here is inserted the actions which take place when a
    ** terminal or non-terminal is destroyed.  This can happen
    ** when the symbol is popped from the stack during a
    ** reduce or during error processing or when a parser is 
    ** being destroyed before it is finished parsing.
    **
    ** Note: during a reduce, the only symbols destroyed are those
    ** which appear on the RHS of the rule, but which are *not* used
    ** inside the C code.
    */
/********* Begin destructor definitions ***************************************/
      /* TERMINAL Destructor */
    case 1: /* AGAINST */
    case 2: /* BETWEEN */
    case 3: /* EQUALS */
    case 4: /* NOTEQUALS */
    case 5: /* LESS */
    case 6: /* GREATER */
    case 7: /* GREATEREQUAL */
    case 8: /* LESSEQUAL */
    case 9: /* TS_MATCHES */
    case 10: /* TS_OR */
    case 11: /* TS_AND */
    case 12: /* TS_NEGATE */
    case 13: /* TS_CONTAINS_ANOTHER */
    case 14: /* TS_CONTAINS_IN */
    case 15: /* AND */
    case 16: /* OR */
    case 17: /* LIKE */
    case 18: /* ILIKE */
    case 19: /* BITWISE_AND */
    case 20: /* BITWISE_OR */
    case 21: /* BITWISE_XOR */
    case 22: /* DIVIDE */
    case 23: /* TIMES */
    case 24: /* MOD */
    case 25: /* PLUS */
    case 26: /* MINUS */
    case 27: /* IS */
    case 28: /* IN */
    case 29: /* NOT */
    case 30: /* BITWISE_NOT */
    case 31: /* COMMA */
    case 32: /* SELECT */
    case 33: /* FROM */
    case 34: /* DISTINCT */
    case 35: /* ALL */
    case 36: /* IDENTIFIER */
    case 37: /* DOT */
    case 38: /* AS */
    case 39: /* INNER */
    case 40: /* JOIN */
    case 41: /* CROSS */
    case 42: /* LEFT */
    case 43: /* OUTER */
    case 44: /* RIGHT */
    case 45: /* FULL */
    case 46: /* ON */
    case 47: /* PARENTHESES_OPEN */
    case 48: /* PARENTHESES_CLOSE */
    case 49: /* INSERT */
    case 50: /* INTO */
    case 51: /* VALUES */
    case 52: /* UPDATE */
    case 53: /* SET */
    case 54: /* DELETE */
    case 55: /* WHERE */
    case 56: /* ORDER */
    case 57: /* BY */
    case 58: /* ASC */
    case 59: /* DESC */
    case 60: /* GROUP */
    case 61: /* HAVING */
    case 62: /* LIMIT */
    case 63: /* OFFSET */
    case 64: /* FOR */
    case 65: /* HINTEGER */
    case 66: /* INTEGER */
    case 67: /* NPLACEHOLDER */
    case 68: /* SPLACEHOLDER */
    case 69: /* EXISTS */
    case 70: /* CAST */
    case 71: /* CONVERT */
    case 72: /* USING */
    case 73: /* CASE */
    case 74: /* END */
    case 75: /* WHEN */
    case 76: /* THEN */
    case 77: /* ELSE */
    case 78: /* FORCEINDEX */
    case 79: /* NULL */
    case 80: /* STRING */
    case 81: /* DOUBLE */
    case 82: /* TRUE */
    case 83: /* FALSE */
    case 84: /* NTPLACEHOLDER */
    case 85: /* STPLACEHOLDER */
    case 86: /* COLON */
{
/* #line 22 "parser.y" */

	if ((yypminor->yy0)) {
		if ((yypminor->yy0)->free_flag) {
			efree((yypminor->yy0)->token);
		}
		efree((yypminor->yy0));
	}

/* #line 1386 "parser.c" */
}
      break;
      /* Default NON-TERMINAL Destructor */
    case 87: /* error */
    case 88: /* program */
    case 89: /* query_language */
    case 90: /* select_statement */
    case 91: /* insert_statement */
    case 92: /* update_statement */
    case 93: /* delete_statement */
    case 94: /* select_clause */
    case 95: /* where_clause */
    case 96: /* group_clause */
    case 97: /* having_clause */
    case 98: /* order_clause */
    case 99: /* select_limit_clause */
    case 100: /* forupdate_clause */
    case 101: /* distinct_all */
    case 102: /* column_list */
    case 103: /* associated_name_list */
    case 104: /* force_index */
    case 105: /* join_list_or_null */
    case 106: /* column_item */
    case 107: /* expr */
    case 108: /* associated_name */
    case 109: /* join_list */
    case 110: /* join_item */
    case 111: /* join_clause */
    case 112: /* join_type */
    case 113: /* aliased_or_qualified_name */
    case 114: /* join_associated_name */
    case 115: /* join_conditions */
    case 116: /* values_list */
    case 117: /* field_list */
    case 118: /* value_item */
    case 119: /* field_item */
    case 120: /* update_clause */
    case 121: /* limit_clause */
    case 122: /* update_item_list */
    case 123: /* update_item */
    case 124: /* qualified_name */
    case 125: /* new_value */
    case 126: /* delete_clause */
    case 127: /* order_list */
    case 128: /* order_item */
    case 129: /* group_list */
    case 130: /* group_item */
    case 131: /* integer_or_placeholder */
    case 132: /* argument_list */
    case 133: /* when_clauses */
    case 134: /* when_clause */
    case 135: /* distinct_or_null */
    case 136: /* argument_list_or_null */
    case 137: /* function_call */
    case 138: /* argument_item */
{
/* #line 31 "parser.y" */

	if ((yypminor->yy92)) {
		if (status) {
			// TODO:
		}
		zval_ptr_dtor(&(yypminor->yy92));
		efree((yypminor->yy92));
	}

/* #line 1453 "parser.c" */
}
      break;
/********* End destructor definitions *****************************************/
    default:  break;   /* If no destructor action specified: do nothing */
  }
}

/*
** Pop the parser's stack once.
**
** If there is a destructor routine associated with the token which
** is popped from the stack, then call it.
*/
static void yy_pop_parser_stack(yyParser *pParser){
  yyStackEntry *yytos;
  assert( pParser->yyidx>=0 );
  yytos = &pParser->yystack[pParser->yyidx--];
#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sPopping %s\n",
      yyTracePrompt,
      yyTokenName[yytos->major]);
  }
#endif
  yy_destructor(pParser, yytos->major, &yytos->minor);
}

/* 
** Deallocate and destroy a parser.  Destructors are called for
** all stack elements before shutting the parser down.
**
** If the YYPARSEFREENEVERNULL macro exists (for example because it
** is defined in a %include section of the input grammar) then it is
** assumed that the input pointer is never NULL.
*/
void phql_Free(
  void *p,                    /* The parser to be deleted */
  void (*freeProc)(void*)     /* Function used to reclaim memory */
){
  yyParser *pParser = (yyParser*)p;
#ifndef YYPARSEFREENEVERNULL
  if( pParser==0 ) return;
#endif
  while( pParser->yyidx>=0 ) yy_pop_parser_stack(pParser);
#if YYSTACKDEPTH<=0
  free(pParser->yystack);
#endif
  (*freeProc)((void*)pParser);
}

/*
** Return the peak depth of the stack for a parser.
*/
#ifdef YYTRACKMAXSTACKDEPTH
int phql_StackPeak(void *p){
  yyParser *pParser = (yyParser*)p;
  return pParser->yyidxMax;
}
#endif

/*
** Find the appropriate action for a parser given the terminal
** look-ahead token iLookAhead.
*/
static unsigned int yy_find_shift_action(
  yyParser *pParser,        /* The parser */
  YYCODETYPE iLookAhead     /* The look-ahead token */
){
  int i;
  int stateno = pParser->yystack[pParser->yyidx].stateno;
 
  if( stateno>=YY_MIN_REDUCE ) return stateno;
  assert( stateno <= YY_SHIFT_COUNT );
  do{
    i = yy_shift_ofst[stateno];
    if( i==YY_SHIFT_USE_DFLT ) return yy_default[stateno];
    assert( iLookAhead!=YYNOCODE );
    i += iLookAhead;
    if( i<0 || i>=YY_ACTTAB_COUNT || yy_lookahead[i]!=iLookAhead ){
      if( iLookAhead>0 ){
#ifdef YYFALLBACK
        YYCODETYPE iFallback;            /* Fallback token */
        if( iLookAhead<sizeof(yyFallback)/sizeof(yyFallback[0])
               && (iFallback = yyFallback[iLookAhead])!=0 ){
#ifndef NDEBUG
          if( yyTraceFILE ){
            fprintf(yyTraceFILE, "%sFALLBACK %s => %s\n",
               yyTracePrompt, yyTokenName[iLookAhead], yyTokenName[iFallback]);
          }
#endif
          assert( yyFallback[iFallback]==0 ); /* Fallback loop must terminate */
          iLookAhead = iFallback;
          continue;
        }
#endif
#ifdef YYWILDCARD
        {
          int j = i - iLookAhead + YYWILDCARD;
          if( 
#if YY_SHIFT_MIN+YYWILDCARD<0
            j>=0 &&
#endif
#if YY_SHIFT_MAX+YYWILDCARD>=YY_ACTTAB_COUNT
            j<YY_ACTTAB_COUNT &&
#endif
            yy_lookahead[j]==YYWILDCARD
          ){
#ifndef NDEBUG
            if( yyTraceFILE ){
              fprintf(yyTraceFILE, "%sWILDCARD %s => %s\n",
                 yyTracePrompt, yyTokenName[iLookAhead],
                 yyTokenName[YYWILDCARD]);
            }
#endif /* NDEBUG */
            return yy_action[j];
          }
        }
#endif /* YYWILDCARD */
      }
      return yy_default[stateno];
    }else{
      return yy_action[i];
    }
  }while(1);
}

/*
** Find the appropriate action for a parser given the non-terminal
** look-ahead token iLookAhead.
*/
static int yy_find_reduce_action(
  int stateno,              /* Current state number */
  YYCODETYPE iLookAhead     /* The look-ahead token */
){
  int i;
#ifdef YYERRORSYMBOL
  if( stateno>YY_REDUCE_COUNT ){
    return yy_default[stateno];
  }
#else
  assert( stateno<=YY_REDUCE_COUNT );
#endif
  i = yy_reduce_ofst[stateno];
  assert( i!=YY_REDUCE_USE_DFLT );
  assert( iLookAhead!=YYNOCODE );
  i += iLookAhead;
#ifdef YYERRORSYMBOL
  if( i<0 || i>=YY_ACTTAB_COUNT || yy_lookahead[i]!=iLookAhead ){
    return yy_default[stateno];
  }
#else
  assert( i>=0 && i<YY_ACTTAB_COUNT );
  assert( yy_lookahead[i]==iLookAhead );
#endif
  return yy_action[i];
}

/*
** The following routine is called if the stack overflows.
*/
static void yyStackOverflow(yyParser *yypParser){
   phql_ARG_FETCH;
   yypParser->yyidx--;
#ifndef NDEBUG
   if( yyTraceFILE ){
     fprintf(yyTraceFILE,"%sStack Overflow!\n",yyTracePrompt);
   }
#endif
   while( yypParser->yyidx>=0 ) yy_pop_parser_stack(yypParser);
   /* Here code is inserted which will execute if the parser
   ** stack every overflows */
/******** Begin %stack_overflow code ******************************************/
/******** End %stack_overflow code ********************************************/
   phql_ARG_STORE; /* Suppress warning about unused %extra_argument var */
}

/*
** Print tracing information for a SHIFT action
*/
#ifndef NDEBUG
static void yyTraceShift(yyParser *yypParser, int yyNewState){
  if( yyTraceFILE ){
    if( yyNewState<YYNSTATE ){
      fprintf(yyTraceFILE,"%sShift '%s', go to state %d\n",
         yyTracePrompt,yyTokenName[yypParser->yystack[yypParser->yyidx].major],
         yyNewState);
    }else{
      fprintf(yyTraceFILE,"%sShift '%s'\n",
         yyTracePrompt,yyTokenName[yypParser->yystack[yypParser->yyidx].major]);
    }
  }
}
#else
# define yyTraceShift(X,Y)
#endif

/*
** Perform a shift action.
*/
static void yy_shift(
  yyParser *yypParser,          /* The parser to be shifted */
  int yyNewState,               /* The new state to shift in */
  int yyMajor,                  /* The major token to shift in */
  phql_TOKENTYPE yyMinor        /* The minor token to shift in */
){
  yyStackEntry *yytos;
  yypParser->yyidx++;
#ifdef YYTRACKMAXSTACKDEPTH
  if( yypParser->yyidx>yypParser->yyidxMax ){
    yypParser->yyidxMax = yypParser->yyidx;
  }
#endif
#if YYSTACKDEPTH>0 
  if( yypParser->yyidx>=YYSTACKDEPTH ){
    yyStackOverflow(yypParser);
    return;
  }
#else
  if( yypParser->yyidx>=yypParser->yystksz ){
    yyGrowStack(yypParser);
    if( yypParser->yyidx>=yypParser->yystksz ){
      yyStackOverflow(yypParser);
      return;
    }
  }
#endif
  yytos = &yypParser->yystack[yypParser->yyidx];
  yytos->stateno = (YYACTIONTYPE)yyNewState;
  yytos->major = (YYCODETYPE)yyMajor;
  yytos->minor.yy0 = yyMinor;
  yyTraceShift(yypParser, yyNewState);
}

/* The following table contains information about every rule that
** is used during the reduce.
*/
static const struct {
  YYCODETYPE lhs;         /* Symbol on the left-hand side of the rule */
  unsigned char nrhs;     /* Number of right-hand side symbols in the rule */
} yyRuleInfo[] = {
  { 88, 1 },
  { 89, 1 },
  { 89, 1 },
  { 89, 1 },
  { 89, 1 },
  { 90, 7 },
  { 94, 7 },
  { 94, 6 },
  { 101, 1 },
  { 101, 1 },
  { 101, 0 },
  { 102, 3 },
  { 102, 1 },
  { 106, 1 },
  { 106, 3 },
  { 106, 3 },
  { 106, 2 },
  { 106, 1 },
  { 103, 3 },
  { 103, 1 },
  { 105, 1 },
  { 105, 0 },
  { 109, 2 },
  { 109, 1 },
  { 110, 1 },
  { 111, 4 },
  { 114, 2 },
  { 114, 1 },
  { 114, 0 },
  { 112, 2 },
  { 112, 2 },
  { 112, 3 },
  { 112, 2 },
  { 112, 3 },
  { 112, 2 },
  { 112, 3 },
  { 112, 2 },
  { 112, 1 },
  { 115, 2 },
  { 115, 0 },
  { 91, 5 },
  { 91, 7 },
  { 91, 10 },
  { 116, 3 },
  { 116, 1 },
  { 118, 1 },
  { 117, 3 },
  { 117, 1 },
  { 119, 1 },
  { 92, 3 },
  { 120, 4 },
  { 122, 3 },
  { 122, 1 },
  { 123, 3 },
  { 125, 1 },
  { 93, 3 },
  { 126, 3 },
  { 108, 3 },
  { 108, 2 },
  { 108, 1 },
  { 113, 1 },
  { 95, 2 },
  { 95, 0 },
  { 98, 3 },
  { 98, 0 },
  { 127, 3 },
  { 127, 1 },
  { 128, 1 },
  { 128, 2 },
  { 128, 2 },
  { 96, 3 },
  { 96, 0 },
  { 129, 3 },
  { 129, 1 },
  { 130, 1 },
  { 97, 2 },
  { 97, 0 },
  { 99, 2 },
  { 99, 4 },
  { 99, 4 },
  { 99, 0 },
  { 121, 2 },
  { 121, 0 },
  { 100, 2 },
  { 100, 0 },
  { 131, 1 },
  { 131, 1 },
  { 131, 1 },
  { 131, 1 },
  { 107, 2 },
  { 107, 3 },
  { 107, 3 },
  { 107, 3 },
  { 107, 3 },
  { 107, 3 },
  { 107, 3 },
  { 107, 3 },
  { 107, 3 },
  { 107, 3 },
  { 107, 3 },
  { 107, 3 },
  { 107, 3 },
  { 107, 3 },
  { 107, 3 },
  { 107, 3 },
  { 107, 3 },
  { 107, 3 },
  { 107, 3 },
  { 107, 3 },
  { 107, 3 },
  { 107, 3 },
  { 107, 3 },
  { 107, 3 },
  { 107, 4 },
  { 107, 3 },
  { 107, 4 },
  { 107, 5 },
  { 107, 6 },
  { 107, 3 },
  { 107, 5 },
  { 107, 6 },
  { 107, 4 },
  { 107, 3 },
  { 107, 6 },
  { 107, 6 },
  { 107, 4 },
  { 133, 2 },
  { 133, 1 },
  { 134, 4 },
  { 134, 2 },
  { 104, 5 },
  { 107, 1 },
  { 137, 5 },
  { 135, 1 },
  { 135, 0 },
  { 136, 1 },
  { 136, 0 },
  { 132, 3 },
  { 132, 1 },
  { 138, 1 },
  { 138, 1 },
  { 107, 3 },
  { 107, 4 },
  { 107, 3 },
  { 107, 2 },
  { 107, 2 },
  { 107, 3 },
  { 107, 1 },
  { 107, 1 },
  { 107, 1 },
  { 107, 1 },
  { 107, 1 },
  { 107, 1 },
  { 107, 1 },
  { 107, 1 },
  { 107, 1 },
  { 107, 1 },
  { 107, 1 },
  { 107, 1 },
  { 124, 5 },
  { 124, 3 },
  { 124, 3 },
  { 124, 1 },
};

static void yy_accept(yyParser*);  /* Forward Declaration */

/*
** Perform a reduce action and the shift that must immediately
** follow the reduce.
*/
static void yy_reduce(
  yyParser *yypParser,         /* The parser */
  unsigned int yyruleno        /* Number of the rule by which to reduce */
){
  int yygoto;                     /* The next state */
  int yyact;                      /* The next action */
  yyStackEntry *yymsp;            /* The top of the parser's stack */
  int yysize;                     /* Amount to pop the stack */
  phql_ARG_FETCH;
  yymsp = &yypParser->yystack[yypParser->yyidx];
#ifndef NDEBUG
  if( yyTraceFILE && yyruleno<(int)(sizeof(yyRuleName)/sizeof(yyRuleName[0])) ){
    yysize = yyRuleInfo[yyruleno].nrhs;
    fprintf(yyTraceFILE, "%sReduce [%s], go to state %d.\n", yyTracePrompt,
      yyRuleName[yyruleno], yymsp[-yysize].stateno);
  }
#endif /* NDEBUG */

  /* Check that the stack is large enough to grow by a single entry
  ** if the RHS of the rule is empty.  This ensures that there is room
  ** enough on the stack to push the LHS value */
  if( yyRuleInfo[yyruleno].nrhs==0 ){
#ifdef YYTRACKMAXSTACKDEPTH
    if( yypParser->yyidx>yypParser->yyidxMax ){
      yypParser->yyidxMax = yypParser->yyidx;
    }
#endif
#if YYSTACKDEPTH>0 
    if( yypParser->yyidx>=YYSTACKDEPTH-1 ){
      yyStackOverflow(yypParser);
      return;
    }
#else
    if( yypParser->yyidx>=yypParser->yystksz-1 ){
      yyGrowStack(yypParser);
      if( yypParser->yyidx>=yypParser->yystksz-1 ){
        yyStackOverflow(yypParser);
        return;
      }
    }
#endif
  }

  switch( yyruleno ){
  /* Beginning here are the reduction cases.  A typical example
  ** follows:
  **   case 0:
  **  #line <lineno> <grammarfile>
  **     { ... }           // User supplied code
  **  #line <lineno> <thisfile>
  **     break;
  */
/********** Begin reduce actions **********************************************/
        YYMINORTYPE yylhsminor;
      case 0: /* program ::= query_language */
/* #line 594 "parser.y" */
{
	status->ret = yymsp[0].minor.yy92;
}
/* #line 1924 "parser.c" */
        break;
      case 1: /* query_language ::= select_statement */
      case 2: /* query_language ::= insert_statement */ yytestcase(yyruleno==2);
      case 3: /* query_language ::= update_statement */ yytestcase(yyruleno==3);
      case 4: /* query_language ::= delete_statement */ yytestcase(yyruleno==4);
      case 19: /* associated_name_list ::= associated_name */ yytestcase(yyruleno==19);
      case 20: /* join_list_or_null ::= join_list */ yytestcase(yyruleno==20);
      case 23: /* join_list ::= join_item */ yytestcase(yyruleno==23);
      case 24: /* join_item ::= join_clause */ yytestcase(yyruleno==24);
      case 45: /* value_item ::= expr */ yytestcase(yyruleno==45);
      case 52: /* update_item_list ::= update_item */ yytestcase(yyruleno==52);
      case 54: /* new_value ::= expr */ yytestcase(yyruleno==54);
      case 60: /* aliased_or_qualified_name ::= qualified_name */ yytestcase(yyruleno==60);
      case 66: /* order_list ::= order_item */ yytestcase(yyruleno==66);
      case 73: /* group_list ::= group_item */ yytestcase(yyruleno==73);
      case 74: /* group_item ::= expr */ yytestcase(yyruleno==74);
      case 131: /* expr ::= function_call */ yytestcase(yyruleno==131);
      case 135: /* argument_list_or_null ::= argument_list */ yytestcase(yyruleno==135);
      case 140: /* argument_item ::= expr */ yytestcase(yyruleno==140);
      case 147: /* expr ::= qualified_name */ yytestcase(yyruleno==147);
/* #line 598 "parser.y" */
{
	yylhsminor.yy92 = yymsp[0].minor.yy92;
}
/* #line 1949 "parser.c" */
  yymsp[0].minor.yy92 = yylhsminor.yy92;
        break;
      case 5: /* select_statement ::= select_clause where_clause group_clause having_clause order_clause select_limit_clause forupdate_clause */
/* #line 614 "parser.y" */
{
	yylhsminor.yy92 = phql_ret_select_statement(yymsp[-6].minor.yy92, yymsp[-5].minor.yy92, yymsp[-2].minor.yy92, yymsp[-4].minor.yy92, yymsp[-3].minor.yy92, yymsp[-1].minor.yy92, yymsp[0].minor.yy92);
}
/* #line 1957 "parser.c" */
  yymsp[-6].minor.yy92 = yylhsminor.yy92;
        break;
      case 6: /* select_clause ::= SELECT distinct_all column_list FROM associated_name_list force_index join_list_or_null */
{  yy_destructor(yypParser,32,&yymsp[-6].minor);
/* #line 618 "parser.y" */
{
	yymsp[-6].minor.yy92 = phql_ret_select_clause(yymsp[-5].minor.yy92, yymsp[-4].minor.yy92, yymsp[-2].minor.yy92, yymsp[0].minor.yy92, yymsp[-1].minor.yy92);
}
/* #line 1966 "parser.c" */
  yy_destructor(yypParser,33,&yymsp[-3].minor);
}
        break;
      case 7: /* select_clause ::= SELECT distinct_all column_list FROM associated_name_list join_list_or_null */
{  yy_destructor(yypParser,32,&yymsp[-5].minor);
/* #line 622 "parser.y" */
{
	yymsp[-5].minor.yy92 = phql_ret_select_clause(yymsp[-4].minor.yy92, yymsp[-3].minor.yy92, yymsp[-1].minor.yy92, yymsp[0].minor.yy92, NULL);
}
/* #line 1976 "parser.c" */
  yy_destructor(yypParser,33,&yymsp[-2].minor);
}
        break;
      case 8: /* distinct_all ::= DISTINCT */
{  yy_destructor(yypParser,34,&yymsp[0].minor);
/* #line 626 "parser.y" */
{
	yymsp[0].minor.yy92 = phql_ret_distinct_all(1);
}
/* #line 1986 "parser.c" */
}
        break;
      case 9: /* distinct_all ::= ALL */
{  yy_destructor(yypParser,35,&yymsp[0].minor);
/* #line 630 "parser.y" */
{
	yymsp[0].minor.yy92 = phql_ret_distinct_all(0);
}
/* #line 1995 "parser.c" */
}
        break;
      case 10: /* distinct_all ::= */
      case 21: /* join_list_or_null ::= */ yytestcase(yyruleno==21);
      case 28: /* join_associated_name ::= */ yytestcase(yyruleno==28);
      case 39: /* join_conditions ::= */ yytestcase(yyruleno==39);
      case 62: /* where_clause ::= */ yytestcase(yyruleno==62);
      case 64: /* order_clause ::= */ yytestcase(yyruleno==64);
      case 71: /* group_clause ::= */ yytestcase(yyruleno==71);
      case 76: /* having_clause ::= */ yytestcase(yyruleno==76);
      case 80: /* select_limit_clause ::= */ yytestcase(yyruleno==80);
      case 82: /* limit_clause ::= */ yytestcase(yyruleno==82);
      case 84: /* forupdate_clause ::= */ yytestcase(yyruleno==84);
      case 134: /* distinct_or_null ::= */ yytestcase(yyruleno==134);
      case 136: /* argument_list_or_null ::= */ yytestcase(yyruleno==136);
/* #line 634 "parser.y" */
{
	yymsp[1].minor.yy92 = NULL;
}
/* #line 2015 "parser.c" */
        break;
      case 11: /* column_list ::= column_list COMMA column_item */
      case 18: /* associated_name_list ::= associated_name_list COMMA associated_name */ yytestcase(yyruleno==18);
      case 43: /* values_list ::= values_list COMMA value_item */ yytestcase(yyruleno==43);
      case 46: /* field_list ::= field_list COMMA field_item */ yytestcase(yyruleno==46);
      case 51: /* update_item_list ::= update_item_list COMMA update_item */ yytestcase(yyruleno==51);
      case 65: /* order_list ::= order_list COMMA order_item */ yytestcase(yyruleno==65);
      case 72: /* group_list ::= group_list COMMA group_item */ yytestcase(yyruleno==72);
      case 137: /* argument_list ::= argument_list COMMA argument_item */ yytestcase(yyruleno==137);
/* #line 638 "parser.y" */
{
	yylhsminor.yy92 = phql_ret_zval_list(yymsp[-2].minor.yy92, yymsp[0].minor.yy92);
}
/* #line 2029 "parser.c" */
  yy_destructor(yypParser,31,&yymsp[-1].minor);
  yymsp[-2].minor.yy92 = yylhsminor.yy92;
        break;
      case 12: /* column_list ::= column_item */
      case 44: /* values_list ::= value_item */ yytestcase(yyruleno==44);
      case 47: /* field_list ::= field_item */ yytestcase(yyruleno==47);
      case 127: /* when_clauses ::= when_clause */ yytestcase(yyruleno==127);
      case 138: /* argument_list ::= argument_item */ yytestcase(yyruleno==138);
/* #line 642 "parser.y" */
{
	yylhsminor.yy92 = phql_ret_zval_list(yymsp[0].minor.yy92, NULL);
}
/* #line 2042 "parser.c" */
  yymsp[0].minor.yy92 = yylhsminor.yy92;
        break;
      case 13: /* column_item ::= TIMES */
      case 139: /* argument_item ::= TIMES */ yytestcase(yyruleno==139);
{  yy_destructor(yypParser,23,&yymsp[0].minor);
/* #line 646 "parser.y" */
{
	yymsp[0].minor.yy92 = phql_ret_column_item(PHQL_T_STARALL, NULL, NULL, NULL);
}
/* #line 2052 "parser.c" */
}
        break;
      case 14: /* column_item ::= IDENTIFIER DOT TIMES */
/* #line 650 "parser.y" */
{
	yylhsminor.yy92 = phql_ret_column_item(PHQL_T_DOMAINALL, NULL, yymsp[-2].minor.yy0, NULL);
}
/* #line 2060 "parser.c" */
  yy_destructor(yypParser,37,&yymsp[-1].minor);
  yy_destructor(yypParser,23,&yymsp[0].minor);
  yymsp[-2].minor.yy92 = yylhsminor.yy92;
        break;
      case 15: /* column_item ::= expr AS IDENTIFIER */
/* #line 654 "parser.y" */
{
	yylhsminor.yy92 = phql_ret_column_item(PHQL_T_EXPR, yymsp[-2].minor.yy92, NULL, yymsp[0].minor.yy0);
}
/* #line 2070 "parser.c" */
  yy_destructor(yypParser,38,&yymsp[-1].minor);
  yymsp[-2].minor.yy92 = yylhsminor.yy92;
        break;
      case 16: /* column_item ::= expr IDENTIFIER */
/* #line 658 "parser.y" */
{
	yylhsminor.yy92 = phql_ret_column_item(PHQL_T_EXPR, yymsp[-1].minor.yy92, NULL, yymsp[0].minor.yy0);
}
/* #line 2079 "parser.c" */
  yymsp[-1].minor.yy92 = yylhsminor.yy92;
        break;
      case 17: /* column_item ::= expr */
/* #line 662 "parser.y" */
{
	yylhsminor.yy92 = phql_ret_column_item(PHQL_T_EXPR, yymsp[0].minor.yy92, NULL, NULL);
}
/* #line 2087 "parser.c" */
  yymsp[0].minor.yy92 = yylhsminor.yy92;
        break;
      case 22: /* join_list ::= join_list join_item */
      case 126: /* when_clauses ::= when_clauses when_clause */ yytestcase(yyruleno==126);
/* #line 682 "parser.y" */
{
	yylhsminor.yy92 = phql_ret_zval_list(yymsp[-1].minor.yy92, yymsp[0].minor.yy92);
}
/* #line 2096 "parser.c" */
  yymsp[-1].minor.yy92 = yylhsminor.yy92;
        break;
      case 25: /* join_clause ::= join_type aliased_or_qualified_name join_associated_name join_conditions */
/* #line 695 "parser.y" */
{
	yylhsminor.yy92 = phql_ret_join_item(yymsp[-3].minor.yy92, yymsp[-2].minor.yy92, yymsp[-1].minor.yy92, yymsp[0].minor.yy92);
}
/* #line 2104 "parser.c" */
  yymsp[-3].minor.yy92 = yylhsminor.yy92;
        break;
      case 26: /* join_associated_name ::= AS IDENTIFIER */
{  yy_destructor(yypParser,38,&yymsp[-1].minor);
/* #line 699 "parser.y" */
{
	yymsp[-1].minor.yy92 = phql_ret_qualified_name(NULL, NULL, yymsp[0].minor.yy0);
}
/* #line 2113 "parser.c" */
}
        break;
      case 27: /* join_associated_name ::= IDENTIFIER */
      case 48: /* field_item ::= IDENTIFIER */ yytestcase(yyruleno==48);
      case 162: /* qualified_name ::= IDENTIFIER */ yytestcase(yyruleno==162);
/* #line 703 "parser.y" */
{
	yylhsminor.yy92 = phql_ret_qualified_name(NULL, NULL, yymsp[0].minor.yy0);
}
/* #line 2123 "parser.c" */
  yymsp[0].minor.yy92 = yylhsminor.yy92;
        break;
      case 29: /* join_type ::= INNER JOIN */
{  yy_destructor(yypParser,39,&yymsp[-1].minor);
/* #line 711 "parser.y" */
{
	yymsp[-1].minor.yy92 = phql_ret_join_type(PHQL_T_INNERJOIN);
}
/* #line 2132 "parser.c" */
  yy_destructor(yypParser,40,&yymsp[0].minor);
}
        break;
      case 30: /* join_type ::= CROSS JOIN */
{  yy_destructor(yypParser,41,&yymsp[-1].minor);
/* #line 715 "parser.y" */
{
	yymsp[-1].minor.yy92 = phql_ret_join_type(PHQL_T_CROSSJOIN);
}
/* #line 2142 "parser.c" */
  yy_destructor(yypParser,40,&yymsp[0].minor);
}
        break;
      case 31: /* join_type ::= LEFT OUTER JOIN */
{  yy_destructor(yypParser,42,&yymsp[-2].minor);
/* #line 719 "parser.y" */
{
	yymsp[-2].minor.yy92 = phql_ret_join_type(PHQL_T_LEFTJOIN);
}
/* #line 2152 "parser.c" */
  yy_destructor(yypParser,43,&yymsp[-1].minor);
  yy_destructor(yypParser,40,&yymsp[0].minor);
}
        break;
      case 32: /* join_type ::= LEFT JOIN */
{  yy_destructor(yypParser,42,&yymsp[-1].minor);
/* #line 723 "parser.y" */
{
	yymsp[-1].minor.yy92 = phql_ret_join_type(PHQL_T_LEFTJOIN);
}
/* #line 2163 "parser.c" */
  yy_destructor(yypParser,40,&yymsp[0].minor);
}
        break;
      case 33: /* join_type ::= RIGHT OUTER JOIN */
{  yy_destructor(yypParser,44,&yymsp[-2].minor);
/* #line 727 "parser.y" */
{
	yymsp[-2].minor.yy92 = phql_ret_join_type(PHQL_T_RIGHTJOIN);
}
/* #line 2173 "parser.c" */
  yy_destructor(yypParser,43,&yymsp[-1].minor);
  yy_destructor(yypParser,40,&yymsp[0].minor);
}
        break;
      case 34: /* join_type ::= RIGHT JOIN */
{  yy_destructor(yypParser,44,&yymsp[-1].minor);
/* #line 731 "parser.y" */
{
	yymsp[-1].minor.yy92 = phql_ret_join_type(PHQL_T_RIGHTJOIN);
}
/* #line 2184 "parser.c" */
  yy_destructor(yypParser,40,&yymsp[0].minor);
}
        break;
      case 35: /* join_type ::= FULL OUTER JOIN */
{  yy_destructor(yypParser,45,&yymsp[-2].minor);
/* #line 735 "parser.y" */
{
	yymsp[-2].minor.yy92 = phql_ret_join_type(PHQL_T_FULLJOIN);
}
/* #line 2194 "parser.c" */
  yy_destructor(yypParser,43,&yymsp[-1].minor);
  yy_destructor(yypParser,40,&yymsp[0].minor);
}
        break;
      case 36: /* join_type ::= FULL JOIN */
{  yy_destructor(yypParser,45,&yymsp[-1].minor);
/* #line 739 "parser.y" */
{
	yymsp[-1].minor.yy92 = phql_ret_join_type(PHQL_T_FULLJOIN);
}
/* #line 2205 "parser.c" */
  yy_destructor(yypParser,40,&yymsp[0].minor);
}
        break;
      case 37: /* join_type ::= JOIN */
{  yy_destructor(yypParser,40,&yymsp[0].minor);
/* #line 743 "parser.y" */
{
	yymsp[0].minor.yy92 = phql_ret_join_type(PHQL_T_INNERJOIN);
}
/* #line 2215 "parser.c" */
}
        break;
      case 38: /* join_conditions ::= ON expr */
      case 61: /* where_clause ::= WHERE expr */ yytestcase(yyruleno==61);
      case 75: /* having_clause ::= HAVING expr */ yytestcase(yyruleno==75);
{  yy_destructor(yypParser,46,&yymsp[-1].minor);
/* #line 747 "parser.y" */
{
	yymsp[-1].minor.yy92 = yymsp[0].minor.yy92;
}
/* #line 2226 "parser.c" */
}
        break;
      case 40: /* insert_statement ::= insert_statement COMMA PARENTHESES_OPEN values_list PARENTHESES_CLOSE */
/* #line 756 "parser.y" */
{
	yylhsminor.yy92 = phql_ret_insert_statement2(yymsp[-4].minor.yy92, NULL, yymsp[-1].minor.yy92);
}
/* #line 2234 "parser.c" */
  yy_destructor(yypParser,31,&yymsp[-3].minor);
  yy_destructor(yypParser,47,&yymsp[-2].minor);
  yy_destructor(yypParser,48,&yymsp[0].minor);
  yymsp[-4].minor.yy92 = yylhsminor.yy92;
        break;
      case 41: /* insert_statement ::= INSERT INTO aliased_or_qualified_name VALUES PARENTHESES_OPEN values_list PARENTHESES_CLOSE */
{  yy_destructor(yypParser,49,&yymsp[-6].minor);
/* #line 760 "parser.y" */
{
	yymsp[-6].minor.yy92 = phql_ret_insert_statement(yymsp[-4].minor.yy92, NULL, yymsp[-1].minor.yy92);
}
/* #line 2246 "parser.c" */
  yy_destructor(yypParser,50,&yymsp[-5].minor);
  yy_destructor(yypParser,51,&yymsp[-3].minor);
  yy_destructor(yypParser,47,&yymsp[-2].minor);
  yy_destructor(yypParser,48,&yymsp[0].minor);
}
        break;
      case 42: /* insert_statement ::= INSERT INTO aliased_or_qualified_name PARENTHESES_OPEN field_list PARENTHESES_CLOSE VALUES PARENTHESES_OPEN values_list PARENTHESES_CLOSE */
{  yy_destructor(yypParser,49,&yymsp[-9].minor);
/* #line 764 "parser.y" */
{
	yymsp[-9].minor.yy92 = phql_ret_insert_statement(yymsp[-7].minor.yy92, yymsp[-5].minor.yy92, yymsp[-1].minor.yy92);
}
/* #line 2259 "parser.c" */
  yy_destructor(yypParser,50,&yymsp[-8].minor);
  yy_destructor(yypParser,47,&yymsp[-6].minor);
  yy_destructor(yypParser,48,&yymsp[-4].minor);
  yy_destructor(yypParser,51,&yymsp[-3].minor);
  yy_destructor(yypParser,47,&yymsp[-2].minor);
  yy_destructor(yypParser,48,&yymsp[0].minor);
}
        break;
      case 49: /* update_statement ::= update_clause where_clause limit_clause */
/* #line 793 "parser.y" */
{
	yylhsminor.yy92 = phql_ret_update_statement(yymsp[-2].minor.yy92, yymsp[-1].minor.yy92, yymsp[0].minor.yy92);
}
/* #line 2273 "parser.c" */
  yymsp[-2].minor.yy92 = yylhsminor.yy92;
        break;
      case 50: /* update_clause ::= UPDATE associated_name SET update_item_list */
{  yy_destructor(yypParser,52,&yymsp[-3].minor);
/* #line 797 "parser.y" */
{
	yymsp[-3].minor.yy92 = phql_ret_update_clause(yymsp[-2].minor.yy92, yymsp[0].minor.yy92);
}
/* #line 2282 "parser.c" */
  yy_destructor(yypParser,53,&yymsp[-1].minor);
}
        break;
      case 53: /* update_item ::= qualified_name EQUALS new_value */
/* #line 809 "parser.y" */
{
	yylhsminor.yy92 = phql_ret_update_item(yymsp[-2].minor.yy92, yymsp[0].minor.yy92);
}
/* #line 2291 "parser.c" */
  yy_destructor(yypParser,3,&yymsp[-1].minor);
  yymsp[-2].minor.yy92 = yylhsminor.yy92;
        break;
      case 55: /* delete_statement ::= delete_clause where_clause limit_clause */
/* #line 818 "parser.y" */
{
	yylhsminor.yy92 = phql_ret_delete_statement(yymsp[-2].minor.yy92, yymsp[-1].minor.yy92, yymsp[0].minor.yy92);
}
/* #line 2300 "parser.c" */
  yymsp[-2].minor.yy92 = yylhsminor.yy92;
        break;
      case 56: /* delete_clause ::= DELETE FROM associated_name */
{  yy_destructor(yypParser,54,&yymsp[-2].minor);
/* #line 822 "parser.y" */
{
	yymsp[-2].minor.yy92 = phql_ret_delete_clause(yymsp[0].minor.yy92);
}
/* #line 2309 "parser.c" */
  yy_destructor(yypParser,33,&yymsp[-1].minor);
}
        break;
      case 57: /* associated_name ::= aliased_or_qualified_name AS IDENTIFIER */
/* #line 826 "parser.y" */
{
	yylhsminor.yy92 = phql_ret_assoc_name(yymsp[-2].minor.yy92, yymsp[0].minor.yy0);
}
/* #line 2318 "parser.c" */
  yy_destructor(yypParser,38,&yymsp[-1].minor);
  yymsp[-2].minor.yy92 = yylhsminor.yy92;
        break;
      case 58: /* associated_name ::= aliased_or_qualified_name IDENTIFIER */
/* #line 830 "parser.y" */
{
	yylhsminor.yy92 = phql_ret_assoc_name(yymsp[-1].minor.yy92, yymsp[0].minor.yy0);
}
/* #line 2327 "parser.c" */
  yymsp[-1].minor.yy92 = yylhsminor.yy92;
        break;
      case 59: /* associated_name ::= aliased_or_qualified_name */
/* #line 834 "parser.y" */
{
	yylhsminor.yy92 = phql_ret_assoc_name(yymsp[0].minor.yy92, NULL);
}
/* #line 2335 "parser.c" */
  yymsp[0].minor.yy92 = yylhsminor.yy92;
        break;
      case 63: /* order_clause ::= ORDER BY order_list */
      case 70: /* group_clause ::= GROUP BY group_list */ yytestcase(yyruleno==70);
{  yy_destructor(yypParser,56,&yymsp[-2].minor);
/* #line 850 "parser.y" */
{
	yymsp[-2].minor.yy92 = yymsp[0].minor.yy92;
}
/* #line 2345 "parser.c" */
  yy_destructor(yypParser,57,&yymsp[-1].minor);
}
        break;
      case 67: /* order_item ::= expr */
/* #line 866 "parser.y" */
{
	yylhsminor.yy92 = phql_ret_order_item(yymsp[0].minor.yy92, 0);
}
/* #line 2354 "parser.c" */
  yymsp[0].minor.yy92 = yylhsminor.yy92;
        break;
      case 68: /* order_item ::= expr ASC */
/* #line 870 "parser.y" */
{
	yylhsminor.yy92 = phql_ret_order_item(yymsp[-1].minor.yy92, PHQL_T_ASC);
}
/* #line 2362 "parser.c" */
  yy_destructor(yypParser,58,&yymsp[0].minor);
  yymsp[-1].minor.yy92 = yylhsminor.yy92;
        break;
      case 69: /* order_item ::= expr DESC */
/* #line 874 "parser.y" */
{
	yylhsminor.yy92 = phql_ret_order_item(yymsp[-1].minor.yy92, PHQL_T_DESC);
}
/* #line 2371 "parser.c" */
  yy_destructor(yypParser,59,&yymsp[0].minor);
  yymsp[-1].minor.yy92 = yylhsminor.yy92;
        break;
      case 77: /* select_limit_clause ::= LIMIT integer_or_placeholder */
      case 81: /* limit_clause ::= LIMIT integer_or_placeholder */ yytestcase(yyruleno==81);
{  yy_destructor(yypParser,62,&yymsp[-1].minor);
/* #line 906 "parser.y" */
{
	yymsp[-1].minor.yy92 = phql_ret_limit_clause(yymsp[0].minor.yy92, NULL);
}
/* #line 2382 "parser.c" */
}
        break;
      case 78: /* select_limit_clause ::= LIMIT integer_or_placeholder COMMA integer_or_placeholder */
{  yy_destructor(yypParser,62,&yymsp[-3].minor);
/* #line 910 "parser.y" */
{
	yymsp[-3].minor.yy92 = phql_ret_limit_clause(yymsp[0].minor.yy92, yymsp[-2].minor.yy92);
}
/* #line 2391 "parser.c" */
  yy_destructor(yypParser,31,&yymsp[-1].minor);
}
        break;
      case 79: /* select_limit_clause ::= LIMIT integer_or_placeholder OFFSET integer_or_placeholder */
{  yy_destructor(yypParser,62,&yymsp[-3].minor);
/* #line 914 "parser.y" */
{
	yymsp[-3].minor.yy92 = phql_ret_limit_clause(yymsp[-2].minor.yy92, yymsp[0].minor.yy92);
}
/* #line 2401 "parser.c" */
  yy_destructor(yypParser,63,&yymsp[-1].minor);
}
        break;
      case 83: /* forupdate_clause ::= FOR UPDATE */
{  yy_destructor(yypParser,64,&yymsp[-1].minor);
/* #line 930 "parser.y" */
{
	yymsp[-1].minor.yy92 = phql_ret_forupdate_clause();
}
/* #line 2411 "parser.c" */
  yy_destructor(yypParser,52,&yymsp[0].minor);
}
        break;
      case 85: /* integer_or_placeholder ::= HINTEGER */
      case 148: /* expr ::= HINTEGER */ yytestcase(yyruleno==148);
/* #line 938 "parser.y" */
{
	yylhsminor.yy92 = phql_ret_literal_zval(PHQL_T_HINTEGER, yymsp[0].minor.yy0);
}
/* #line 2421 "parser.c" */
  yymsp[0].minor.yy92 = yylhsminor.yy92;
        break;
      case 86: /* integer_or_placeholder ::= INTEGER */
      case 149: /* expr ::= INTEGER */ yytestcase(yyruleno==149);
/* #line 942 "parser.y" */
{
	yylhsminor.yy92 = phql_ret_literal_zval(PHQL_T_INTEGER, yymsp[0].minor.yy0);
}
/* #line 2430 "parser.c" */
  yymsp[0].minor.yy92 = yylhsminor.yy92;
        break;
      case 87: /* integer_or_placeholder ::= NPLACEHOLDER */
      case 155: /* expr ::= NPLACEHOLDER */ yytestcase(yyruleno==155);
/* #line 946 "parser.y" */
{
	yylhsminor.yy92 = phql_ret_placeholder_zval(PHQL_T_NPLACEHOLDER, yymsp[0].minor.yy0);
}
/* #line 2439 "parser.c" */
  yymsp[0].minor.yy92 = yylhsminor.yy92;
        break;
      case 88: /* integer_or_placeholder ::= SPLACEHOLDER */
      case 156: /* expr ::= SPLACEHOLDER */ yytestcase(yyruleno==156);
/* #line 950 "parser.y" */
{
	yylhsminor.yy92 = phql_ret_placeholder_zval(PHQL_T_SPLACEHOLDER, yymsp[0].minor.yy0);
}
/* #line 2448 "parser.c" */
  yymsp[0].minor.yy92 = yylhsminor.yy92;
        break;
      case 89: /* expr ::= MINUS expr */
{  yy_destructor(yypParser,26,&yymsp[-1].minor);
/* #line 954 "parser.y" */
{
	yymsp[-1].minor.yy92 = phql_ret_expr(PHQL_T_MINUS, NULL, yymsp[0].minor.yy92);
}
/* #line 2457 "parser.c" */
}
        break;
      case 90: /* expr ::= expr MINUS expr */
/* #line 958 "parser.y" */
{
	yylhsminor.yy92 = phql_ret_expr(PHQL_T_SUB, yymsp[-2].minor.yy92, yymsp[0].minor.yy92);
}
/* #line 2465 "parser.c" */
  yy_destructor(yypParser,26,&yymsp[-1].minor);
  yymsp[-2].minor.yy92 = yylhsminor.yy92;
        break;
      case 91: /* expr ::= expr PLUS expr */
/* #line 962 "parser.y" */
{
	yylhsminor.yy92 = phql_ret_expr(PHQL_T_ADD, yymsp[-2].minor.yy92, yymsp[0].minor.yy92);
}
/* #line 2474 "parser.c" */
  yy_destructor(yypParser,25,&yymsp[-1].minor);
  yymsp[-2].minor.yy92 = yylhsminor.yy92;
        break;
      case 92: /* expr ::= expr TIMES expr */
/* #line 966 "parser.y" */
{
	yylhsminor.yy92 = phql_ret_expr(PHQL_T_MUL, yymsp[-2].minor.yy92, yymsp[0].minor.yy92);
}
/* #line 2483 "parser.c" */
  yy_destructor(yypParser,23,&yymsp[-1].minor);
  yymsp[-2].minor.yy92 = yylhsminor.yy92;
        break;
      case 93: /* expr ::= expr DIVIDE expr */
/* #line 970 "parser.y" */
{
	yylhsminor.yy92 = phql_ret_expr(PHQL_T_DIV, yymsp[-2].minor.yy92, yymsp[0].minor.yy92);
}
/* #line 2492 "parser.c" */
  yy_destructor(yypParser,22,&yymsp[-1].minor);
  yymsp[-2].minor.yy92 = yylhsminor.yy92;
        break;
      case 94: /* expr ::= expr MOD expr */
/* #line 974 "parser.y" */
{
	yylhsminor.yy92 = phql_ret_expr(PHQL_T_MOD, yymsp[-2].minor.yy92, yymsp[0].minor.yy92);
}
/* #line 2501 "parser.c" */
  yy_destructor(yypParser,24,&yymsp[-1].minor);
  yymsp[-2].minor.yy92 = yylhsminor.yy92;
        break;
      case 95: /* expr ::= expr AND expr */
/* #line 978 "parser.y" */
{
	yylhsminor.yy92 = phql_ret_expr(PHQL_T_AND, yymsp[-2].minor.yy92, yymsp[0].minor.yy92);
}
/* #line 2510 "parser.c" */
  yy_destructor(yypParser,15,&yymsp[-1].minor);
  yymsp[-2].minor.yy92 = yylhsminor.yy92;
        break;
      case 96: /* expr ::= expr OR expr */
/* #line 982 "parser.y" */
{
	yylhsminor.yy92 = phql_ret_expr(PHQL_T_OR, yymsp[-2].minor.yy92, yymsp[0].minor.yy92);
}
/* #line 2519 "parser.c" */
  yy_destructor(yypParser,16,&yymsp[-1].minor);
  yymsp[-2].minor.yy92 = yylhsminor.yy92;
        break;
      case 97: /* expr ::= expr BITWISE_AND expr */
/* #line 986 "parser.y" */
{
	yylhsminor.yy92 = phql_ret_expr(PHQL_T_BITWISE_AND, yymsp[-2].minor.yy92, yymsp[0].minor.yy92);
}
/* #line 2528 "parser.c" */
  yy_destructor(yypParser,19,&yymsp[-1].minor);
  yymsp[-2].minor.yy92 = yylhsminor.yy92;
        break;
      case 98: /* expr ::= expr BITWISE_OR expr */
/* #line 990 "parser.y" */
{
	yylhsminor.yy92 = phql_ret_expr(PHQL_T_BITWISE_OR, yymsp[-2].minor.yy92, yymsp[0].minor.yy92);
}
/* #line 2537 "parser.c" */
  yy_destructor(yypParser,20,&yymsp[-1].minor);
  yymsp[-2].minor.yy92 = yylhsminor.yy92;
        break;
      case 99: /* expr ::= expr BITWISE_XOR expr */
/* #line 994 "parser.y" */
{
	yylhsminor.yy92 = phql_ret_expr(PHQL_T_BITWISE_XOR, yymsp[-2].minor.yy92, yymsp[0].minor.yy92);
}
/* #line 2546 "parser.c" */
  yy_destructor(yypParser,21,&yymsp[-1].minor);
  yymsp[-2].minor.yy92 = yylhsminor.yy92;
        break;
      case 100: /* expr ::= expr EQUALS expr */
/* #line 998 "parser.y" */
{
	yylhsminor.yy92 = phql_ret_expr(PHQL_T_EQUALS, yymsp[-2].minor.yy92, yymsp[0].minor.yy92);
}
/* #line 2555 "parser.c" */
  yy_destructor(yypParser,3,&yymsp[-1].minor);
  yymsp[-2].minor.yy92 = yylhsminor.yy92;
        break;
      case 101: /* expr ::= expr NOTEQUALS expr */
/* #line 1002 "parser.y" */
{
	yylhsminor.yy92 = phql_ret_expr(PHQL_T_NOTEQUALS, yymsp[-2].minor.yy92, yymsp[0].minor.yy92);
}
/* #line 2564 "parser.c" */
  yy_destructor(yypParser,4,&yymsp[-1].minor);
  yymsp[-2].minor.yy92 = yylhsminor.yy92;
        break;
      case 102: /* expr ::= expr LESS expr */
/* #line 1006 "parser.y" */
{
	yylhsminor.yy92 = phql_ret_expr(PHQL_T_LESS, yymsp[-2].minor.yy92, yymsp[0].minor.yy92);
}
/* #line 2573 "parser.c" */
  yy_destructor(yypParser,5,&yymsp[-1].minor);
  yymsp[-2].minor.yy92 = yylhsminor.yy92;
        break;
      case 103: /* expr ::= expr GREATER expr */
/* #line 1010 "parser.y" */
{
	yylhsminor.yy92 = phql_ret_expr(PHQL_T_GREATER, yymsp[-2].minor.yy92, yymsp[0].minor.yy92);
}
/* #line 2582 "parser.c" */
  yy_destructor(yypParser,6,&yymsp[-1].minor);
  yymsp[-2].minor.yy92 = yylhsminor.yy92;
        break;
      case 104: /* expr ::= expr GREATEREQUAL expr */
/* #line 1014 "parser.y" */
{
	yylhsminor.yy92 = phql_ret_expr(PHQL_T_GREATEREQUAL, yymsp[-2].minor.yy92, yymsp[0].minor.yy92);
}
/* #line 2591 "parser.c" */
  yy_destructor(yypParser,7,&yymsp[-1].minor);
  yymsp[-2].minor.yy92 = yylhsminor.yy92;
        break;
      case 105: /* expr ::= expr TS_MATCHES expr */
/* #line 1018 "parser.y" */
{
	yylhsminor.yy92 = phql_ret_expr(PHQL_T_TS_MATCHES, yymsp[-2].minor.yy92, yymsp[0].minor.yy92);
}
/* #line 2600 "parser.c" */
  yy_destructor(yypParser,9,&yymsp[-1].minor);
  yymsp[-2].minor.yy92 = yylhsminor.yy92;
        break;
      case 106: /* expr ::= expr TS_OR expr */
/* #line 1022 "parser.y" */
{
	yylhsminor.yy92 = phql_ret_expr(PHQL_T_TS_OR, yymsp[-2].minor.yy92, yymsp[0].minor.yy92);
}
/* #line 2609 "parser.c" */
  yy_destructor(yypParser,10,&yymsp[-1].minor);
  yymsp[-2].minor.yy92 = yylhsminor.yy92;
        break;
      case 107: /* expr ::= expr TS_AND expr */
/* #line 1026 "parser.y" */
{
	yylhsminor.yy92 = phql_ret_expr(PHQL_T_TS_AND, yymsp[-2].minor.yy92, yymsp[0].minor.yy92);
}
/* #line 2618 "parser.c" */
  yy_destructor(yypParser,11,&yymsp[-1].minor);
  yymsp[-2].minor.yy92 = yylhsminor.yy92;
        break;
      case 108: /* expr ::= expr TS_NEGATE expr */
/* #line 1030 "parser.y" */
{
	yylhsminor.yy92 = phql_ret_expr(PHQL_T_TS_NEGATE, yymsp[-2].minor.yy92, yymsp[0].minor.yy92);
}
/* #line 2627 "parser.c" */
  yy_destructor(yypParser,12,&yymsp[-1].minor);
  yymsp[-2].minor.yy92 = yylhsminor.yy92;
        break;
      case 109: /* expr ::= expr TS_CONTAINS_ANOTHER expr */
/* #line 1034 "parser.y" */
{
	yylhsminor.yy92 = phql_ret_expr(PHQL_T_TS_CONTAINS_ANOTHER, yymsp[-2].minor.yy92, yymsp[0].minor.yy92);
}
/* #line 2636 "parser.c" */
  yy_destructor(yypParser,13,&yymsp[-1].minor);
  yymsp[-2].minor.yy92 = yylhsminor.yy92;
        break;
      case 110: /* expr ::= expr TS_CONTAINS_IN expr */
/* #line 1038 "parser.y" */
{
	yylhsminor.yy92 = phql_ret_expr(PHQL_T_TS_CONTAINS_IN, yymsp[-2].minor.yy92, yymsp[0].minor.yy92);
}
/* #line 2645 "parser.c" */
  yy_destructor(yypParser,14,&yymsp[-1].minor);
  yymsp[-2].minor.yy92 = yylhsminor.yy92;
        break;
      case 111: /* expr ::= expr LESSEQUAL expr */
/* #line 1042 "parser.y" */
{
	yylhsminor.yy92 = phql_ret_expr(PHQL_T_LESSEQUAL, yymsp[-2].minor.yy92, yymsp[0].minor.yy92);
}
/* #line 2654 "parser.c" */
  yy_destructor(yypParser,8,&yymsp[-1].minor);
  yymsp[-2].minor.yy92 = yylhsminor.yy92;
        break;
      case 112: /* expr ::= expr LIKE expr */
/* #line 1046 "parser.y" */
{
	yylhsminor.yy92 = phql_ret_expr(PHQL_T_LIKE, yymsp[-2].minor.yy92, yymsp[0].minor.yy92);
}
/* #line 2663 "parser.c" */
  yy_destructor(yypParser,17,&yymsp[-1].minor);
  yymsp[-2].minor.yy92 = yylhsminor.yy92;
        break;
      case 113: /* expr ::= expr NOT LIKE expr */
/* #line 1050 "parser.y" */
{
	yylhsminor.yy92 = phql_ret_expr(PHQL_T_NLIKE, yymsp[-3].minor.yy92, yymsp[0].minor.yy92);
}
/* #line 2672 "parser.c" */
  yy_destructor(yypParser,29,&yymsp[-2].minor);
  yy_destructor(yypParser,17,&yymsp[-1].minor);
  yymsp[-3].minor.yy92 = yylhsminor.yy92;
        break;
      case 114: /* expr ::= expr ILIKE expr */
/* #line 1054 "parser.y" */
{
	yylhsminor.yy92 = phql_ret_expr(PHQL_T_ILIKE, yymsp[-2].minor.yy92, yymsp[0].minor.yy92);
}
/* #line 2682 "parser.c" */
  yy_destructor(yypParser,18,&yymsp[-1].minor);
  yymsp[-2].minor.yy92 = yylhsminor.yy92;
        break;
      case 115: /* expr ::= expr NOT ILIKE expr */
/* #line 1058 "parser.y" */
{
	yylhsminor.yy92 = phql_ret_expr(PHQL_T_NILIKE, yymsp[-3].minor.yy92, yymsp[0].minor.yy92);
}
/* #line 2691 "parser.c" */
  yy_destructor(yypParser,29,&yymsp[-2].minor);
  yy_destructor(yypParser,18,&yymsp[-1].minor);
  yymsp[-3].minor.yy92 = yylhsminor.yy92;
        break;
      case 116: /* expr ::= expr IN PARENTHESES_OPEN argument_list PARENTHESES_CLOSE */
      case 119: /* expr ::= expr IN PARENTHESES_OPEN select_statement PARENTHESES_CLOSE */ yytestcase(yyruleno==119);
/* #line 1062 "parser.y" */
{
	yylhsminor.yy92 = phql_ret_expr(PHQL_T_IN, yymsp[-4].minor.yy92, yymsp[-1].minor.yy92);
}
/* #line 2702 "parser.c" */
  yy_destructor(yypParser,28,&yymsp[-3].minor);
  yy_destructor(yypParser,47,&yymsp[-2].minor);
  yy_destructor(yypParser,48,&yymsp[0].minor);
  yymsp[-4].minor.yy92 = yylhsminor.yy92;
        break;
      case 117: /* expr ::= expr NOT IN PARENTHESES_OPEN argument_list PARENTHESES_CLOSE */
      case 120: /* expr ::= expr NOT IN PARENTHESES_OPEN select_statement PARENTHESES_CLOSE */ yytestcase(yyruleno==120);
/* #line 1066 "parser.y" */
{
	yylhsminor.yy92 = phql_ret_expr(PHQL_T_NOTIN, yymsp[-5].minor.yy92, yymsp[-1].minor.yy92);
}
/* #line 2714 "parser.c" */
  yy_destructor(yypParser,29,&yymsp[-4].minor);
  yy_destructor(yypParser,28,&yymsp[-3].minor);
  yy_destructor(yypParser,47,&yymsp[-2].minor);
  yy_destructor(yypParser,48,&yymsp[0].minor);
  yymsp[-5].minor.yy92 = yylhsminor.yy92;
        break;
      case 118: /* expr ::= PARENTHESES_OPEN select_statement PARENTHESES_CLOSE */
{  yy_destructor(yypParser,47,&yymsp[-2].minor);
/* #line 1070 "parser.y" */
{
	yymsp[-2].minor.yy92 = phql_ret_expr(PHQL_T_SUBQUERY, yymsp[-1].minor.yy92, NULL);
}
/* #line 2727 "parser.c" */
  yy_destructor(yypParser,48,&yymsp[0].minor);
}
        break;
      case 121: /* expr ::= EXISTS PARENTHESES_OPEN select_statement PARENTHESES_CLOSE */
{  yy_destructor(yypParser,69,&yymsp[-3].minor);
/* #line 1082 "parser.y" */
{
	yymsp[-3].minor.yy92 = phql_ret_expr(PHQL_T_EXISTS, NULL, yymsp[-1].minor.yy92);
}
/* #line 2737 "parser.c" */
  yy_destructor(yypParser,47,&yymsp[-2].minor);
  yy_destructor(yypParser,48,&yymsp[0].minor);
}
        break;
      case 122: /* expr ::= expr AGAINST expr */
/* #line 1086 "parser.y" */
{
	yylhsminor.yy92 = phql_ret_expr(PHQL_T_AGAINST, yymsp[-2].minor.yy92, yymsp[0].minor.yy92);
}
/* #line 2747 "parser.c" */
  yy_destructor(yypParser,1,&yymsp[-1].minor);
  yymsp[-2].minor.yy92 = yylhsminor.yy92;
        break;
      case 123: /* expr ::= CAST PARENTHESES_OPEN expr AS IDENTIFIER PARENTHESES_CLOSE */
{  yy_destructor(yypParser,70,&yymsp[-5].minor);
/* #line 1090 "parser.y" */
{
	yymsp[-5].minor.yy92 = phql_ret_expr(PHQL_T_CAST, yymsp[-3].minor.yy92, phql_ret_raw_qualified_name(yymsp[-1].minor.yy0, NULL));
}
/* #line 2757 "parser.c" */
  yy_destructor(yypParser,47,&yymsp[-4].minor);
  yy_destructor(yypParser,38,&yymsp[-2].minor);
  yy_destructor(yypParser,48,&yymsp[0].minor);
}
        break;
      case 124: /* expr ::= CONVERT PARENTHESES_OPEN expr USING IDENTIFIER PARENTHESES_CLOSE */
{  yy_destructor(yypParser,71,&yymsp[-5].minor);
/* #line 1094 "parser.y" */
{
	yymsp[-5].minor.yy92 = phql_ret_expr(PHQL_T_CONVERT, yymsp[-3].minor.yy92, phql_ret_raw_qualified_name(yymsp[-1].minor.yy0, NULL));
}
/* #line 2769 "parser.c" */
  yy_destructor(yypParser,47,&yymsp[-4].minor);
  yy_destructor(yypParser,72,&yymsp[-2].minor);
  yy_destructor(yypParser,48,&yymsp[0].minor);
}
        break;
      case 125: /* expr ::= CASE expr when_clauses END */
{  yy_destructor(yypParser,73,&yymsp[-3].minor);
/* #line 1098 "parser.y" */
{
	yymsp[-3].minor.yy92 = phql_ret_expr(PHQL_T_CASE, yymsp[-2].minor.yy92, yymsp[-1].minor.yy92);
}
/* #line 2781 "parser.c" */
  yy_destructor(yypParser,74,&yymsp[0].minor);
}
        break;
      case 128: /* when_clause ::= WHEN expr THEN expr */
{  yy_destructor(yypParser,75,&yymsp[-3].minor);
/* #line 1110 "parser.y" */
{
	yymsp[-3].minor.yy92 = phql_ret_expr(PHQL_T_WHEN, yymsp[-2].minor.yy92, yymsp[0].minor.yy92);
}
/* #line 2791 "parser.c" */
  yy_destructor(yypParser,76,&yymsp[-1].minor);
}
        break;
      case 129: /* when_clause ::= ELSE expr */
{  yy_destructor(yypParser,77,&yymsp[-1].minor);
/* #line 1114 "parser.y" */
{
	yymsp[-1].minor.yy92 = phql_ret_expr(PHQL_T_ELSE, yymsp[0].minor.yy92, NULL);
}
/* #line 2801 "parser.c" */
}
        break;
      case 130: /* force_index ::= FORCEINDEX PARENTHESES_OPEN distinct_or_null argument_list_or_null PARENTHESES_CLOSE */
      case 132: /* function_call ::= IDENTIFIER PARENTHESES_OPEN distinct_or_null argument_list_or_null PARENTHESES_CLOSE */ yytestcase(yyruleno==132);
/* #line 1118 "parser.y" */
{
	yylhsminor.yy92 = phql_ret_func_call(yymsp[-4].minor.yy0, yymsp[-1].minor.yy92, yymsp[-2].minor.yy92);
}
/* #line 2810 "parser.c" */
  yy_destructor(yypParser,47,&yymsp[-3].minor);
  yy_destructor(yypParser,48,&yymsp[0].minor);
  yymsp[-4].minor.yy92 = yylhsminor.yy92;
        break;
      case 133: /* distinct_or_null ::= DISTINCT */
{  yy_destructor(yypParser,34,&yymsp[0].minor);
/* #line 1130 "parser.y" */
{
	yymsp[0].minor.yy92 = phql_ret_distinct();
}
/* #line 2821 "parser.c" */
}
        break;
      case 141: /* expr ::= expr IS NULL */
/* #line 1162 "parser.y" */
{
	yylhsminor.yy92 = phql_ret_expr(PHQL_T_ISNULL, yymsp[-2].minor.yy92, NULL);
}
/* #line 2829 "parser.c" */
  yy_destructor(yypParser,27,&yymsp[-1].minor);
  yy_destructor(yypParser,79,&yymsp[0].minor);
  yymsp[-2].minor.yy92 = yylhsminor.yy92;
        break;
      case 142: /* expr ::= expr IS NOT NULL */
/* #line 1166 "parser.y" */
{
	yylhsminor.yy92 = phql_ret_expr(PHQL_T_ISNOTNULL, yymsp[-3].minor.yy92, NULL);
}
/* #line 2839 "parser.c" */
  yy_destructor(yypParser,27,&yymsp[-2].minor);
  yy_destructor(yypParser,29,&yymsp[-1].minor);
  yy_destructor(yypParser,79,&yymsp[0].minor);
  yymsp[-3].minor.yy92 = yylhsminor.yy92;
        break;
      case 143: /* expr ::= expr BETWEEN expr */
/* #line 1170 "parser.y" */
{
	yylhsminor.yy92 = phql_ret_expr(PHQL_T_BETWEEN, yymsp[-2].minor.yy92, yymsp[0].minor.yy92);
}
/* #line 2850 "parser.c" */
  yy_destructor(yypParser,2,&yymsp[-1].minor);
  yymsp[-2].minor.yy92 = yylhsminor.yy92;
        break;
      case 144: /* expr ::= NOT expr */
{  yy_destructor(yypParser,29,&yymsp[-1].minor);
/* #line 1174 "parser.y" */
{
	yymsp[-1].minor.yy92 = phql_ret_expr(PHQL_T_NOT, NULL, yymsp[0].minor.yy92);
}
/* #line 2860 "parser.c" */
}
        break;
      case 145: /* expr ::= BITWISE_NOT expr */
{  yy_destructor(yypParser,30,&yymsp[-1].minor);
/* #line 1178 "parser.y" */
{
	yymsp[-1].minor.yy92 = phql_ret_expr(PHQL_T_BITWISE_NOT, NULL, yymsp[0].minor.yy92);
}
/* #line 2869 "parser.c" */
}
        break;
      case 146: /* expr ::= PARENTHESES_OPEN expr PARENTHESES_CLOSE */
{  yy_destructor(yypParser,47,&yymsp[-2].minor);
/* #line 1182 "parser.y" */
{
	yymsp[-2].minor.yy92 = phql_ret_expr(PHQL_T_ENCLOSED, yymsp[-1].minor.yy92, NULL);
}
/* #line 2878 "parser.c" */
  yy_destructor(yypParser,48,&yymsp[0].minor);
}
        break;
      case 150: /* expr ::= STRING */
/* #line 1198 "parser.y" */
{
	yylhsminor.yy92 = phql_ret_literal_zval(PHQL_T_STRING, yymsp[0].minor.yy0);
}
/* #line 2887 "parser.c" */
  yymsp[0].minor.yy92 = yylhsminor.yy92;
        break;
      case 151: /* expr ::= DOUBLE */
/* #line 1202 "parser.y" */
{
	yylhsminor.yy92 = phql_ret_literal_zval(PHQL_T_DOUBLE, yymsp[0].minor.yy0);
}
/* #line 2895 "parser.c" */
  yymsp[0].minor.yy92 = yylhsminor.yy92;
        break;
      case 152: /* expr ::= NULL */
{  yy_destructor(yypParser,79,&yymsp[0].minor);
/* #line 1206 "parser.y" */
{
	yymsp[0].minor.yy92 = phql_ret_literal_zval(PHQL_T_NULL, NULL);
}
/* #line 2904 "parser.c" */
}
        break;
      case 153: /* expr ::= TRUE */
{  yy_destructor(yypParser,82,&yymsp[0].minor);
/* #line 1210 "parser.y" */
{
	yymsp[0].minor.yy92 = phql_ret_literal_zval(PHQL_T_TRUE, NULL);
}
/* #line 2913 "parser.c" */
}
        break;
      case 154: /* expr ::= FALSE */
{  yy_destructor(yypParser,83,&yymsp[0].minor);
/* #line 1214 "parser.y" */
{
	yymsp[0].minor.yy92 = phql_ret_literal_zval(PHQL_T_FALSE, NULL);
}
/* #line 2922 "parser.c" */
}
        break;
      case 157: /* expr ::= NTPLACEHOLDER */
/* #line 1226 "parser.y" */
{
	yylhsminor.yy92 = phql_ret_placeholder_zval(PHQL_T_NTPLACEHOLDER, yymsp[0].minor.yy0);
}
/* #line 2930 "parser.c" */
  yymsp[0].minor.yy92 = yylhsminor.yy92;
        break;
      case 158: /* expr ::= STPLACEHOLDER */
/* #line 1230 "parser.y" */
{
	yylhsminor.yy92 = phql_ret_placeholder_zval(PHQL_T_STPLACEHOLDER, yymsp[0].minor.yy0);
}
/* #line 2938 "parser.c" */
  yymsp[0].minor.yy92 = yylhsminor.yy92;
        break;
      case 159: /* qualified_name ::= IDENTIFIER COLON IDENTIFIER DOT IDENTIFIER */
/* #line 1234 "parser.y" */
{
	yylhsminor.yy92 = phql_ret_qualified_name(yymsp[-4].minor.yy0, yymsp[-2].minor.yy0, yymsp[0].minor.yy0);
}
/* #line 2946 "parser.c" */
  yy_destructor(yypParser,86,&yymsp[-3].minor);
  yy_destructor(yypParser,37,&yymsp[-1].minor);
  yymsp[-4].minor.yy92 = yylhsminor.yy92;
        break;
      case 160: /* qualified_name ::= IDENTIFIER COLON IDENTIFIER */
/* #line 1238 "parser.y" */
{
	yylhsminor.yy92 = phql_ret_qualified_name(yymsp[-2].minor.yy0, NULL, yymsp[0].minor.yy0);
}
/* #line 2956 "parser.c" */
  yy_destructor(yypParser,86,&yymsp[-1].minor);
  yymsp[-2].minor.yy92 = yylhsminor.yy92;
        break;
      case 161: /* qualified_name ::= IDENTIFIER DOT IDENTIFIER */
/* #line 1242 "parser.y" */
{
	yylhsminor.yy92 = phql_ret_qualified_name(NULL, yymsp[-2].minor.yy0, yymsp[0].minor.yy0);
}
/* #line 2965 "parser.c" */
  yy_destructor(yypParser,37,&yymsp[-1].minor);
  yymsp[-2].minor.yy92 = yylhsminor.yy92;
        break;
      default:
        break;
/********** End reduce actions ************************************************/
  };
  assert( yyruleno<sizeof(yyRuleInfo)/sizeof(yyRuleInfo[0]) );
  yygoto = yyRuleInfo[yyruleno].lhs;
  yysize = yyRuleInfo[yyruleno].nrhs;
  yyact = yy_find_reduce_action(yymsp[-yysize].stateno,(YYCODETYPE)yygoto);
  if( yyact <= YY_MAX_SHIFTREDUCE ){
    if( yyact>YY_MAX_SHIFT ) yyact += YY_MIN_REDUCE - YY_MIN_SHIFTREDUCE;
    yypParser->yyidx -= yysize - 1;
    yymsp -= yysize-1;
    yymsp->stateno = (YYACTIONTYPE)yyact;
    yymsp->major = (YYCODETYPE)yygoto;
    yyTraceShift(yypParser, yyact);
  }else{
    assert( yyact == YY_ACCEPT_ACTION );
    yypParser->yyidx -= yysize;
    yy_accept(yypParser);
  }
}

/*
** The following code executes when the parse fails
*/
#ifndef YYNOERRORRECOVERY
static void yy_parse_failed(
  yyParser *yypParser           /* The parser */
){
  phql_ARG_FETCH;
#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sFail!\n",yyTracePrompt);
  }
#endif
  while( yypParser->yyidx>=0 ) yy_pop_parser_stack(yypParser);
  /* Here code is inserted which will be executed whenever the
  ** parser fails */
/************ Begin %parse_failure code ***************************************/
/************ End %parse_failure code *****************************************/
  phql_ARG_STORE; /* Suppress warning about unused %extra_argument variable */
}
#endif /* YYNOERRORRECOVERY */

/*
** The following code executes when a syntax error first occurs.
*/
static void yy_syntax_error(
  yyParser *yypParser,           /* The parser */
  int yymajor,                   /* The major type of the error token */
  phql_TOKENTYPE yyminor         /* The minor type of the error token */
){
  phql_ARG_FETCH;
#define TOKEN yyminor
/************ Begin %syntax_error code ****************************************/
/* #line 527 "parser.y" */

	if (status->scanner_state->start_length) {
		{

			char *token_name = NULL;
			int token_found = 0;
			unsigned int token_length;
			const phql_token_names *tokens = phql_tokens;
			uint active_token = status->scanner_state->active_token;
			uint near_length = status->scanner_state->start_length;

			if (active_token) {

				do {
					if (tokens->code == active_token) {
						token_name = tokens->name;
						token_length = tokens->length;
						token_found = 1;
						break;
					}
					++tokens;
				} while (tokens[0].code != 0);

			}

			if (!token_name) {
				token_length = strlen("UNKNOWN");
				token_name = estrndup("UNKNOWN", token_length);
				token_found = 0;
			}

			status->syntax_error_len = 96 + status->token->len + token_length + near_length + status->phql_length;;
			status->syntax_error = emalloc(sizeof(char) * status->syntax_error_len);

			if (near_length > 0) {
				if (status->token->value) {
					snprintf(status->syntax_error, status->syntax_error_len, "Syntax error, unexpected token %s(%s), near to '%s', when parsing: %s (%d)", token_name, status->token->value, status->scanner_state->start, status->phql, status->phql_length);
				} else {
					snprintf(status->syntax_error, status->syntax_error_len, "Syntax error, unexpected token %s, near to '%s', when parsing: %s (%d)", token_name, status->scanner_state->start, status->phql, status->phql_length);
				}
			} else {
				if (active_token != PHQL_T_IGNORE) {
					if (status->token->value) {
						snprintf(status->syntax_error, status->syntax_error_len, "Syntax error, unexpected token %s(%s), at the end of query, when parsing: %s (%d)", token_name, status->token->value, status->phql, status->phql_length);
					} else {
						snprintf(status->syntax_error, status->syntax_error_len, "Syntax error, unexpected token %s, at the end of query, when parsing: %s (%d)", token_name, status->phql, status->phql_length);
					}
				} else {
					snprintf(status->syntax_error, status->syntax_error_len, "Syntax error, unexpected EOF, at the end of query");
				}
				status->syntax_error[status->syntax_error_len - 1] = '\0';
			}

			if (!token_found) {
				if (token_name) {
					efree(token_name);
				}
			}
		}
	} else {
		status->syntax_error_len = strlen("Syntax error, unexpected EOF");
		status->syntax_error = estrndup("Syntax error, unexpected EOF", status->syntax_error_len);
	}

	status->status = PHQL_PARSING_FAILED;
/* #line 3090 "parser.c" */
/************ End %syntax_error code ******************************************/
  phql_ARG_STORE; /* Suppress warning about unused %extra_argument variable */
}

/*
** The following is executed when the parser accepts
*/
static void yy_accept(
  yyParser *yypParser           /* The parser */
){
  phql_ARG_FETCH;
#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sAccept!\n",yyTracePrompt);
  }
#endif
  while( yypParser->yyidx>=0 ) yy_pop_parser_stack(yypParser);
  /* Here code is inserted which will be executed whenever the
  ** parser accepts */
/*********** Begin %parse_accept code *****************************************/
/*********** End %parse_accept code *******************************************/
  phql_ARG_STORE; /* Suppress warning about unused %extra_argument variable */
}

/* The main parser program.
** The first argument is a pointer to a structure obtained from
** "phql_Alloc" which describes the current state of the parser.
** The second argument is the major token number.  The third is
** the minor token.  The fourth optional argument is whatever the
** user wants (and specified in the grammar) and is available for
** use by the action routines.
**
** Inputs:
** <ul>
** <li> A pointer to the parser (an opaque structure.)
** <li> The major token number.
** <li> The minor token number.
** <li> An option argument of a grammar-specified type.
** </ul>
**
** Outputs:
** None.
*/
void phql_(
  void *yyp,                   /* The parser */
  int yymajor,                 /* The major token code number */
  phql_TOKENTYPE yyminor       /* The value for the token */
  phql_ARG_PDECL               /* Optional %extra_argument parameter */
){
  YYMINORTYPE yyminorunion;
  unsigned int yyact;   /* The parser action. */
#if !defined(YYERRORSYMBOL) && !defined(YYNOERRORRECOVERY)
  int yyendofinput;     /* True if we are at the end of input */
#endif
#ifdef YYERRORSYMBOL
  int yyerrorhit = 0;   /* True if yymajor has invoked an error */
#endif
  yyParser *yypParser;  /* The parser */

  /* (re)initialize the parser, if necessary */
  yypParser = (yyParser*)yyp;
  if( yypParser->yyidx<0 ){
#if YYSTACKDEPTH<=0
    if( yypParser->yystksz <=0 ){
      yyStackOverflow(yypParser);
      return;
    }
#endif
    yypParser->yyidx = 0;
#ifndef YYNOERRORRECOVERY
    yypParser->yyerrcnt = -1;
#endif
    yypParser->yystack[0].stateno = 0;
    yypParser->yystack[0].major = 0;
#ifndef NDEBUG
    if( yyTraceFILE ){
      fprintf(yyTraceFILE,"%sInitialize. Empty stack. State 0\n",
              yyTracePrompt);
    }
#endif
  }
#if !defined(YYERRORSYMBOL) && !defined(YYNOERRORRECOVERY)
  yyendofinput = (yymajor==0);
#endif
  phql_ARG_STORE;

#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sInput '%s'\n",yyTracePrompt,yyTokenName[yymajor]);
  }
#endif

  do{
    yyact = yy_find_shift_action(yypParser,(YYCODETYPE)yymajor);
    if( yyact <= YY_MAX_SHIFTREDUCE ){
      if( yyact > YY_MAX_SHIFT ) yyact += YY_MIN_REDUCE - YY_MIN_SHIFTREDUCE;
      yy_shift(yypParser,yyact,yymajor,yyminor);
#ifndef YYNOERRORRECOVERY
      yypParser->yyerrcnt--;
#endif
      yymajor = YYNOCODE;
    }else if( yyact <= YY_MAX_REDUCE ){
      yy_reduce(yypParser,yyact-YY_MIN_REDUCE);
    }else{
      assert( yyact == YY_ERROR_ACTION );
      yyminorunion.yy0 = yyminor;
#ifdef YYERRORSYMBOL
      int yymx;
#endif
#ifndef NDEBUG
      if( yyTraceFILE ){
        fprintf(yyTraceFILE,"%sSyntax Error!\n",yyTracePrompt);
      }
#endif
#ifdef YYERRORSYMBOL
      /* A syntax error has occurred.
      ** The response to an error depends upon whether or not the
      ** grammar defines an error token "ERROR".  
      **
      ** This is what we do if the grammar does define ERROR:
      **
      **  * Call the %syntax_error function.
      **
      **  * Begin popping the stack until we enter a state where
      **    it is legal to shift the error symbol, then shift
      **    the error symbol.
      **
      **  * Set the error count to three.
      **
      **  * Begin accepting and shifting new tokens.  No new error
      **    processing will occur until three tokens have been
      **    shifted successfully.
      **
      */
      if( yypParser->yyerrcnt<0 ){
        yy_syntax_error(yypParser,yymajor,yyminor);
      }
      yymx = yypParser->yystack[yypParser->yyidx].major;
      if( yymx==YYERRORSYMBOL || yyerrorhit ){
#ifndef NDEBUG
        if( yyTraceFILE ){
          fprintf(yyTraceFILE,"%sDiscard input token %s\n",
             yyTracePrompt,yyTokenName[yymajor]);
        }
#endif
        yy_destructor(yypParser, (YYCODETYPE)yymajor, &yyminorunion);
        yymajor = YYNOCODE;
      }else{
        while(
          yypParser->yyidx >= 0 &&
          yymx != YYERRORSYMBOL &&
          (yyact = yy_find_reduce_action(
                        yypParser->yystack[yypParser->yyidx].stateno,
                        YYERRORSYMBOL)) >= YY_MIN_REDUCE
        ){
          yy_pop_parser_stack(yypParser);
        }
        if( yypParser->yyidx < 0 || yymajor==0 ){
          yy_destructor(yypParser,(YYCODETYPE)yymajor,&yyminorunion);
          yy_parse_failed(yypParser);
          yymajor = YYNOCODE;
        }else if( yymx!=YYERRORSYMBOL ){
          yy_shift(yypParser,yyact,YYERRORSYMBOL,yyminor);
        }
      }
      yypParser->yyerrcnt = 3;
      yyerrorhit = 1;
#elif defined(YYNOERRORRECOVERY)
      /* If the YYNOERRORRECOVERY macro is defined, then do not attempt to
      ** do any kind of error recovery.  Instead, simply invoke the syntax
      ** error routine and continue going as if nothing had happened.
      **
      ** Applications can set this macro (for example inside %include) if
      ** they intend to abandon the parse upon the first syntax error seen.
      */
      yy_syntax_error(yypParser,yymajor, yyminor);
      yy_destructor(yypParser,(YYCODETYPE)yymajor,&yyminorunion);
      yymajor = YYNOCODE;
      
#else  /* YYERRORSYMBOL is not defined */
      /* This is what we do if the grammar does not define ERROR:
      **
      **  * Report an error message, and throw away the input token.
      **
      **  * If the input token is $, then fail the parse.
      **
      ** As before, subsequent error messages are suppressed until
      ** three input tokens have been successfully shifted.
      */
      if( yypParser->yyerrcnt<=0 ){
        yy_syntax_error(yypParser,yymajor, yyminor);
      }
      yypParser->yyerrcnt = 3;
      yy_destructor(yypParser,(YYCODETYPE)yymajor,&yyminorunion);
      if( yyendofinput ){
        yy_parse_failed(yypParser);
      }
      yymajor = YYNOCODE;
#endif
    }
  }while( yymajor!=YYNOCODE && yypParser->yyidx>=0 );
#ifndef NDEBUG
  if( yyTraceFILE ){
    int i;
    fprintf(yyTraceFILE,"%sReturn. Stack=",yyTracePrompt);
    for(i=1; i<=yypParser->yyidx; i++)
      fprintf(yyTraceFILE,"%c%s", i==1 ? '[' : ' ', 
              yyTokenName[yypParser->yystack[i].major]);
    fprintf(yyTraceFILE,"]\n");
  }
#endif
  return;
}
/*
  +------------------------------------------------------------------------+
  | Phalcon Framework                                                      |
  +------------------------------------------------------------------------+
  | Copyright (c) 2011-2014 Phalcon Team (http://www.phalconphp.com)       |
  +------------------------------------------------------------------------+
  | This source file is subject to the New BSD License that is bundled     |
  | with this package in the file docs/LICENSE.txt.                        |
  |                                                                        |
  | If you did not receive a copy of the license and are unable to         |
  | obtain it through the world-wide-web, please send an email             |
  | to license@phalconphp.com so we can send you a copy immediately.       |
  +------------------------------------------------------------------------+
  | Authors: Andres Gutierrez <andres@phalconphp.com>                      |
  |          Eduar Carvajal <eduar@phalconphp.com>                         |
  +------------------------------------------------------------------------+
*/

const phql_token_names phql_tokens[] =
{
  { SL("INTEGER"),       PHQL_T_INTEGER },
  { SL("DOUBLE"),        PHQL_T_DOUBLE },
  { SL("STRING"),        PHQL_T_STRING },
  { SL("IDENTIFIER"),    PHQL_T_IDENTIFIER },
  { SL("MINUS"),         PHQL_T_MINUS },
  { SL("+"),             PHQL_T_ADD },
  { SL("-"),             PHQL_T_SUB },
  { SL("*"),             PHQL_T_MUL },
  { SL("/"),             PHQL_T_DIV },
  { SL("&"),             PHQL_T_BITWISE_AND },
  { SL("|"),             PHQL_T_BITWISE_OR },
  { SL("%%"),            PHQL_T_MOD },
  { SL("AND"),           PHQL_T_AND },
  { SL("OR"),            PHQL_T_OR },
  { SL("LIKE"),          PHQL_T_LIKE },
  { SL("ILIKE"),         PHQL_T_ILIKE },
  { SL("DOT"),           PHQL_T_DOT },
  { SL("COLON"),         PHQL_T_COLON },
  { SL("COMMA"),         PHQL_T_COMMA },
  { SL("EQUALS"),        PHQL_T_EQUALS },
  { SL("NOT EQUALS"),    PHQL_T_NOTEQUALS },
  { SL("NOT"),           PHQL_T_NOT },
  { SL("<"),             PHQL_T_LESS },
  { SL("<="),            PHQL_T_LESSEQUAL },
  { SL(">"),             PHQL_T_GREATER },
  { SL(">="),            PHQL_T_GREATEREQUAL },
  { SL("@@"),            PHQL_T_TS_MATCHES },
  { SL("||"),            PHQL_T_TS_OR },
  { SL("&&"),            PHQL_T_TS_AND },
  { SL("!!"),            PHQL_T_TS_NEGATE },
  { SL("@>"),            PHQL_T_TS_CONTAINS_ANOTHER },
  { SL("<@"),            PHQL_T_TS_CONTAINS_IN },
  { SL("("),             PHQL_T_PARENTHESES_OPEN },
  { SL(")"),             PHQL_T_PARENTHESES_CLOSE },
  { SL("NUMERIC PLACEHOLDER"), PHQL_T_NPLACEHOLDER },
  { SL("STRING PLACEHOLDER"),  PHQL_T_SPLACEHOLDER },
  { SL("UPDATE"),        PHQL_T_UPDATE },
  { SL("SET"),           PHQL_T_SET },
  { SL("WHERE"),         PHQL_T_WHERE },
  { SL("DELETE"),        PHQL_T_DELETE },
  { SL("FROM"),          PHQL_T_FROM },
  { SL("AS"),            PHQL_T_AS },
  { SL("INSERT"),        PHQL_T_INSERT },
  { SL("INTO"),          PHQL_T_INTO },
  { SL("VALUES"),        PHQL_T_VALUES },
  { SL("SELECT"),        PHQL_T_SELECT },
  { SL("ORDER"),         PHQL_T_ORDER },
  { SL("BY"),            PHQL_T_BY },
  { SL("LIMIT"),         PHQL_T_LIMIT },
  { SL("OFFSET"),        PHQL_T_OFFSET },
  { SL("GROUP"),         PHQL_T_GROUP },
  { SL("HAVING"),        PHQL_T_HAVING },
  { SL("IN"),            PHQL_T_IN },
  { SL("ON"),            PHQL_T_ON },
  { SL("INNER"),         PHQL_T_INNER },
  { SL("JOIN"),          PHQL_T_JOIN },
  { SL("LEFT"),          PHQL_T_LEFT },
  { SL("RIGHT"),         PHQL_T_RIGHT },
  { SL("IS"),            PHQL_T_IS },
  { SL("NULL"),          PHQL_T_NULL },
  { SL("NOT IN"),        PHQL_T_NOTIN },
  { SL("CROSS"),         PHQL_T_CROSS },
  { SL("OUTER"),         PHQL_T_OUTER },
  { SL("FULL"),          PHQL_T_FULL },
  { SL("ASC"),           PHQL_T_ASC },
  { SL("DESC"),          PHQL_T_DESC },
  { SL("BETWEEN"),       PHQL_T_BETWEEN },
  { SL("DISTINCT"),      PHQL_T_DISTINCT },
  { SL("AGAINST"),       PHQL_T_AGAINST },
  { SL("CAST"),          PHQL_T_CAST },
  { SL("CONVERT"),       PHQL_T_CONVERT },
  { SL("USING"),         PHQL_T_USING },
  { SL("ALL"),           PHQL_T_ALL },
  { SL("FOR"),           PHQL_T_FOR },
  { SL("EXISTS"),        PHQL_T_EXISTS },
  { SL("CASE"),          PHQL_T_CASE },
  { SL("WHEN"),          PHQL_T_WHEN },
  { SL("THEN"),          PHQL_T_THEN },
  { SL("ELSE"),          PHQL_T_ELSE },
  { SL("END"),           PHQL_T_END },
  { NULL, 0, 0 }
};

static void *phql_wrapper_alloc(size_t bytes){
	return emalloc(bytes);
}

static void phql_wrapper_free(void *pointer){
	efree(pointer);
}

static void phql_parse_with_token(void* phql_parser, int opcode, int parsercode, phql_scanner_token *token, phql_parser_status *parser_status){

	phql_parser_token *pToken;

	pToken = emalloc(sizeof(phql_parser_token));
	pToken->opcode = opcode;
	pToken->token = token->value;
	pToken->token_len = token->len;
	pToken->free_flag = 1;
	phql_(phql_parser, parsercode, pToken, parser_status);

	token->value = NULL;
	token->len = 0;
}

/**
 * Creates an error message when it's triggered by the scanner
 */
static void phql_scanner_error_msg(phql_parser_status *parser_status, zval **error_msg TSRMLS_DC){

	char *error = NULL, *error_part;
	unsigned int length;
	phql_scanner_state *state = parser_status->scanner_state;

	MAKE_STD_ZVAL(*error_msg);
	if (state->start) {
		length = 64 + state->start_length + parser_status->phql_length;
		error = emalloc(sizeof(char) * length);
		if (state->start_length > 16) {
			error_part = estrndup(state->start, 16);
			snprintf(error, length, "Scanning error before '%s...' when parsing: %s (%d)", error_part, parser_status->phql, parser_status->phql_length);
			efree(error_part);
		} else {
			snprintf(error, length, "Scanning error before '%s' when parsing: %s (%d)", state->start, parser_status->phql, parser_status->phql_length);
		}
		error[length - 1] = '\0';
		ZVAL_STRING(*error_msg, error, 1);
	} else {
		ZVAL_STRING(*error_msg, "Scanning error near to EOF", 1);
	}

	if (error) {
		efree(error);
	}
}

/**
 * Executes the internal PHQL parser/tokenizer
 */
int phql_parse_phql(zval *result, zval *phql TSRMLS_DC) {

	zval *error_msg = NULL;

	ZVAL_NULL(result);

	if (phql_internal_parse_phql(&result, Z_STRVAL_P(phql), Z_STRLEN_P(phql), &error_msg TSRMLS_CC) == FAILURE) {
		if (likely(error_msg != NULL)) {
			PHALCON_THROW_EXCEPTION_STRW(phalcon_mvc_model_exception_ce, Z_STRVAL_P(error_msg));
			zval_ptr_dtor(&error_msg);
		}
		else {
			PHALCON_THROW_EXCEPTION_STRW(phalcon_mvc_model_exception_ce, "There was an error parsing PHQL");
		}

		return FAILURE;
	}

	return SUCCESS;
}

/**
 * Executes a PHQL parser/tokenizer
 */
int phql_internal_parse_phql(zval **result, char *phql, unsigned int phql_length, zval **error_msg TSRMLS_DC) {

	zend_phalcon_globals *phalcon_globals_ptr = PHALCON_VGLOBAL;
	phql_parser_status *parser_status = NULL;
	int scanner_status, status = SUCCESS, error_length;
	phql_scanner_state *state;
	phql_scanner_token token;
	void* phql_parser;
	char *error;
	zval *unique_id;

	if (!phql) {
		MAKE_STD_ZVAL(*error_msg);
		ZVAL_STRING(*error_msg, "PHQL statement cannot be NULL", 1);
		return FAILURE;
	}

	MAKE_STD_ZVAL(unique_id);
	ZVAL_LONG(unique_id, zend_inline_hash_func(phql, phql_length + 1));

	phalcon_orm_get_prepared_ast(result, unique_id TSRMLS_CC);

	if (Z_TYPE_PP(result) == IS_ARRAY) {
		zval_ptr_dtor(&unique_id);
		return SUCCESS;
	}

	phql_parser = phql_Alloc(phql_wrapper_alloc);
	if (unlikely(!phql_parser)) {
		MAKE_STD_ZVAL(*error_msg);
		ZVAL_STRING(*error_msg, "Memory allocation error", 1);
		return FAILURE;
	}

	parser_status = emalloc(sizeof(phql_parser_status));
	state = emalloc(sizeof(phql_scanner_state));

	parser_status->status = PHQL_PARSING_OK;
	parser_status->scanner_state = state;
	parser_status->ret = NULL;
	parser_status->syntax_error = NULL;
	parser_status->token = &token;
	parser_status->enable_literals = phalcon_globals_ptr->orm.enable_literals;
	parser_status->phql = phql;
	parser_status->phql_length = phql_length;

	state->active_token = 0;
	state->start = phql;
	state->start_length = 0;
	state->end = state->start;

	token.value = NULL;
	token.len = 0;

	while (0 <= (scanner_status = phql_get_token(state, &token))) {

		/* Calculate the 'start' length */
		state->start_length = (phql + phql_length - state->start);

		state->active_token = token.opcode;

		/* Parse the token found */
		switch (token.opcode) {

			case PHQL_T_IGNORE:
				break;

			case PHQL_T_ADD:
				phql_(phql_parser, PHQL_PLUS, NULL, parser_status);
				break;
			case PHQL_T_SUB:
				phql_(phql_parser, PHQL_MINUS, NULL, parser_status);
				break;
			case PHQL_T_MUL:
				phql_(phql_parser, PHQL_TIMES, NULL, parser_status);
				break;
			case PHQL_T_DIV:
				phql_(phql_parser, PHQL_DIVIDE, NULL, parser_status);
				break;
			case PHQL_T_MOD:
				phql_(phql_parser, PHQL_MOD, NULL, parser_status);
				break;
			case PHQL_T_AND:
				phql_(phql_parser, PHQL_AND, NULL, parser_status);
				break;
			case PHQL_T_OR:
				phql_(phql_parser, PHQL_OR, NULL, parser_status);
				break;
			case PHQL_T_EQUALS:
				phql_(phql_parser, PHQL_EQUALS, NULL, parser_status);
				break;
			case PHQL_T_NOTEQUALS:
				phql_(phql_parser, PHQL_NOTEQUALS, NULL, parser_status);
				break;
			case PHQL_T_LESS:
				phql_(phql_parser, PHQL_LESS, NULL, parser_status);
				break;
			case PHQL_T_GREATER:
				phql_(phql_parser, PHQL_GREATER, NULL, parser_status);
				break;
			case PHQL_T_GREATEREQUAL:
				phql_(phql_parser, PHQL_GREATEREQUAL, NULL, parser_status);
				break;
			case PHQL_T_LESSEQUAL:
				phql_(phql_parser, PHQL_LESSEQUAL, NULL, parser_status);
				break;

            case PHQL_T_IDENTIFIER:
    			phql_parse_with_token(phql_parser, PHQL_T_IDENTIFIER, PHQL_IDENTIFIER, &token, parser_status);
    			break;

            case PHQL_T_DOT:
    			phql_(phql_parser, PHQL_DOT, NULL, parser_status);
    			break;
    		case PHQL_T_COMMA:
    			phql_(phql_parser, PHQL_COMMA, NULL, parser_status);
    			break;

    		case PHQL_T_PARENTHESES_OPEN:
    			phql_(phql_parser, PHQL_PARENTHESES_OPEN, NULL, parser_status);
    			break;
    		case PHQL_T_PARENTHESES_CLOSE:
    			phql_(phql_parser, PHQL_PARENTHESES_CLOSE, NULL, parser_status);
    			break;

			case PHQL_T_LIKE:
				phql_(phql_parser, PHQL_LIKE, NULL, parser_status);
				break;
			case PHQL_T_ILIKE:
				phql_(phql_parser, PHQL_ILIKE, NULL, parser_status);
				break;
			case PHQL_T_NOT:
				phql_(phql_parser, PHQL_NOT, NULL, parser_status);
				break;
			case PHQL_T_BITWISE_AND:
				phql_(phql_parser, PHQL_BITWISE_AND, NULL, parser_status);
				break;
			case PHQL_T_BITWISE_OR:
				phql_(phql_parser, PHQL_BITWISE_OR, NULL, parser_status);
				break;
			case PHQL_T_BITWISE_NOT:
				phql_(phql_parser, PHQL_BITWISE_NOT, NULL, parser_status);
				break;
			case PHQL_T_BITWISE_XOR:
				phql_(phql_parser, PHQL_BITWISE_XOR, NULL, parser_status);
				break;
			case PHQL_T_AGAINST:
				phql_(phql_parser, PHQL_AGAINST, NULL, parser_status);
				break;
            case PHQL_T_CASE:
    			phql_(phql_parser, PHQL_CASE, NULL, parser_status);
    			break;
            case PHQL_T_WHEN:
        		phql_(phql_parser, PHQL_WHEN, NULL, parser_status);
        		break;
            case PHQL_T_THEN:
            	phql_(phql_parser, PHQL_THEN, NULL, parser_status);
            	break;
            case PHQL_T_END:
            	phql_(phql_parser, PHQL_END, NULL, parser_status);
            	break;
            case PHQL_T_ELSE:
                phql_(phql_parser, PHQL_ELSE, NULL, parser_status);
                break;

			case PHQL_T_INTEGER:
				if (parser_status->enable_literals) {
					phql_parse_with_token(phql_parser, PHQL_T_INTEGER, PHQL_INTEGER, &token, parser_status);
				} else {
					MAKE_STD_ZVAL(*error_msg);
					ZVAL_STRING(*error_msg, "Literals are disabled in PHQL statements", 1);
					parser_status->status = PHQL_PARSING_FAILED;
				}
				break;
			case PHQL_T_DOUBLE:
				if (parser_status->enable_literals) {
					phql_parse_with_token(phql_parser, PHQL_T_DOUBLE, PHQL_DOUBLE, &token, parser_status);
				} else {
					MAKE_STD_ZVAL(*error_msg);
					ZVAL_STRING(*error_msg, "Literals are disabled in PHQL statements", 1);
					parser_status->status = PHQL_PARSING_FAILED;
				}
				break;
			case PHQL_T_STRING:
				if (parser_status->enable_literals) {
					phql_parse_with_token(phql_parser, PHQL_T_STRING, PHQL_STRING, &token, parser_status);
				} else {
					MAKE_STD_ZVAL(*error_msg);
					ZVAL_STRING(*error_msg, "Literals are disabled in PHQL statements", 1);
					parser_status->status = PHQL_PARSING_FAILED;
				}
				break;
			case PHQL_T_TRUE:
				if (parser_status->enable_literals) {
					phql_(phql_parser, PHQL_TRUE, NULL, parser_status);
				} else {
					MAKE_STD_ZVAL(*error_msg);
					ZVAL_STRING(*error_msg, "Literals are disabled in PHQL statements", 1);
					parser_status->status = PHQL_PARSING_FAILED;
				}
				break;
			case PHQL_T_FALSE:
				if (parser_status->enable_literals) {
					phql_(phql_parser, PHQL_FALSE, NULL, parser_status);
				} else {
					MAKE_STD_ZVAL(*error_msg);
					ZVAL_STRING(*error_msg, "Literals are disabled in PHQL statements", 1);
					parser_status->status = PHQL_PARSING_FAILED;
				}
				break;

			case PHQL_T_NPLACEHOLDER:
				phql_parse_with_token(phql_parser, PHQL_T_NPLACEHOLDER, PHQL_NPLACEHOLDER, &token, parser_status);
				break;
			case PHQL_T_SPLACEHOLDER:
				phql_parse_with_token(phql_parser, PHQL_T_SPLACEHOLDER, PHQL_SPLACEHOLDER, &token, parser_status);
				break;
            case PHQL_T_NTPLACEHOLDER:
    			phql_parse_with_token(phql_parser, PHQL_T_NTPLACEHOLDER, PHQL_NTPLACEHOLDER, &token, parser_status);
    			break;
    		case PHQL_T_STPLACEHOLDER:
    			phql_parse_with_token(phql_parser, PHQL_T_STPLACEHOLDER, PHQL_STPLACEHOLDER, &token, parser_status);
    			break;

			case PHQL_T_FROM:
				phql_(phql_parser, PHQL_FROM, NULL, parser_status);
				break;
			case PHQL_T_UPDATE:
				phql_(phql_parser, PHQL_UPDATE, NULL, parser_status);
				break;
			case PHQL_T_SET:
				phql_(phql_parser, PHQL_SET, NULL, parser_status);
				break;
			case PHQL_T_WHERE:
				phql_(phql_parser, PHQL_WHERE, NULL, parser_status);
				break;
			case PHQL_T_DELETE:
				phql_(phql_parser, PHQL_DELETE, NULL, parser_status);
				break;
			case PHQL_T_INSERT:
				phql_(phql_parser, PHQL_INSERT, NULL, parser_status);
				break;
			case PHQL_T_INTO:
				phql_(phql_parser, PHQL_INTO, NULL, parser_status);
				break;
			case PHQL_T_VALUES:
				phql_(phql_parser, PHQL_VALUES, NULL, parser_status);
				break;
			case PHQL_T_SELECT:
				phql_(phql_parser, PHQL_SELECT, NULL, parser_status);
				break;
			case PHQL_T_AS:
				phql_(phql_parser, PHQL_AS, NULL, parser_status);
				break;
			case PHQL_T_ORDER:
				phql_(phql_parser, PHQL_ORDER, NULL, parser_status);
				break;
			case PHQL_T_BY:
				phql_(phql_parser, PHQL_BY, NULL, parser_status);
				break;
			case PHQL_T_LIMIT:
				phql_(phql_parser, PHQL_LIMIT, NULL, parser_status);
				break;
			case PHQL_T_OFFSET:
				phql_(phql_parser, PHQL_OFFSET, NULL, parser_status);
				break;
			case PHQL_T_GROUP:
				phql_(phql_parser, PHQL_GROUP, NULL, parser_status);
				break;
			case PHQL_T_HAVING:
				phql_(phql_parser, PHQL_HAVING, NULL, parser_status);
				break;
			case PHQL_T_ASC:
				phql_(phql_parser, PHQL_ASC, NULL, parser_status);
				break;
			case PHQL_T_DESC:
				phql_(phql_parser, PHQL_DESC, NULL, parser_status);
				break;
			case PHQL_T_IN:
				phql_(phql_parser, PHQL_IN, NULL, parser_status);
				break;
			case PHQL_T_ON:
				phql_(phql_parser, PHQL_ON, NULL, parser_status);
				break;
			case PHQL_T_INNER:
				phql_(phql_parser, PHQL_INNER, NULL, parser_status);
				break;
			case PHQL_T_JOIN:
				phql_(phql_parser, PHQL_JOIN, NULL, parser_status);
				break;
			case PHQL_T_LEFT:
				phql_(phql_parser, PHQL_LEFT, NULL, parser_status);
				break;
			case PHQL_T_RIGHT:
				phql_(phql_parser, PHQL_RIGHT, NULL, parser_status);
				break;
			case PHQL_T_CROSS:
				phql_(phql_parser, PHQL_CROSS, NULL, parser_status);
				break;
			case PHQL_T_FULL:
				phql_(phql_parser, PHQL_FULL, NULL, parser_status);
				break;
			case PHQL_T_OUTER:
				phql_(phql_parser, PHQL_OUTER, NULL, parser_status);
				break;
			case PHQL_T_IS:
				phql_(phql_parser, PHQL_IS, NULL, parser_status);
				break;
			case PHQL_T_NULL:
				phql_(phql_parser, PHQL_NULL, NULL, parser_status);
				break;
			case PHQL_T_BETWEEN:
				phql_(phql_parser, PHQL_BETWEEN, NULL, parser_status);
				break;
			case PHQL_T_DISTINCT:
				phql_(phql_parser, PHQL_DISTINCT, NULL, parser_status);
				break;
			case PHQL_T_ALL:
				phql_(phql_parser, PHQL_ALL, NULL, parser_status);
				break;
			case PHQL_T_CAST:
				phql_(phql_parser, PHQL_CAST, NULL, parser_status);
				break;
			case PHQL_T_CONVERT:
				phql_(phql_parser, PHQL_CONVERT, NULL, parser_status);
				break;
			case PHQL_T_USING:
				phql_(phql_parser, PHQL_USING, NULL, parser_status);
				break;
            case PHQL_T_EXISTS:
    			phql_(phql_parser, PHQL_EXISTS, NULL, parser_status);
    			break;
			case PHQL_T_TS_MATCHES:
				phql_(phql_parser, PHQL_TS_MATCHES, NULL, parser_status);
				break;
			case PHQL_T_TS_OR:
				phql_(phql_parser, PHQL_TS_OR, NULL, parser_status);
				break;
			case PHQL_T_TS_AND:
				phql_(phql_parser, PHQL_TS_AND, NULL, parser_status);
				break;
			case PHQL_T_TS_NEGATE:
				phql_(phql_parser, PHQL_TS_NEGATE, NULL, parser_status);
				break;
			case PHQL_T_TS_CONTAINS_ANOTHER:
				phql_(phql_parser, PHQL_TS_CONTAINS_ANOTHER, NULL, parser_status);
				break;
			case PHQL_T_TS_CONTAINS_IN:
				phql_(phql_parser, PHQL_TS_CONTAINS_IN, NULL, parser_status);
				break;
			case PHQL_T_FOR:
				phql_(phql_parser, PHQL_FOR, NULL, parser_status);
				break;
			case PHQL_T_FORCEINDEX:
    			phql_parse_with_token(phql_parser, PHQL_T_FORCEINDEX, PHQL_FORCEINDEX, &token, parser_status);
				break;
			default:
				parser_status->status = PHQL_PARSING_FAILED;
				error_length = sizeof(char) * 32;
				error = emalloc(error_length);
				snprintf(error, error_length, "Scanner: Unknown opcode %c", token.opcode);
				error[error_length - 1] = '\0';
				MAKE_STD_ZVAL(*error_msg);
				ZVAL_STRING(*error_msg, error, 1);
				efree(error);
				break;
		}

		if (parser_status->status != PHQL_PARSING_OK) {
			status = FAILURE;
			break;
		}

		state->end = state->start;
	}

	if (status != FAILURE) {
		switch (scanner_status) {
			case PHQL_SCANNER_RETCODE_ERR:
			case PHQL_SCANNER_RETCODE_IMPOSSIBLE:
				if (!*error_msg) {
					if (!*error_msg) {
						phql_scanner_error_msg(parser_status, error_msg TSRMLS_CC);
					}
				}
				status = FAILURE;
				break;
			default:
				phql_(phql_parser, 0, NULL, parser_status);
		}
	}

	state->active_token = 0;
	state->start = NULL;

	if (parser_status->status != PHQL_PARSING_OK) {
		status = FAILURE;
		if (parser_status->syntax_error) {
			if (!*error_msg) {
				MAKE_STD_ZVAL(*error_msg);
				ZVAL_STRING(*error_msg, parser_status->syntax_error, 1);
			}
			efree(parser_status->syntax_error);
		}
	}

	phql_Free(phql_parser, phql_wrapper_free);

	if (status != FAILURE) {
		if (parser_status->status == PHQL_PARSING_OK) {
			if (parser_status->ret) {

				/**
				 * Set a unique id for the parsed ast
				 */
				if (phalcon_globals_ptr->orm.cache_level >= 1) {
					if (Z_TYPE_P(parser_status->ret) == IS_ARRAY) {
						add_assoc_long(parser_status->ret, "id", Z_LVAL_P(unique_id));
					}
				}

				ZVAL_ZVAL(*result, parser_status->ret, 0, 0);
				ZVAL_NULL(parser_status->ret);
				zval_ptr_dtor(&parser_status->ret);

				/**
				 * Store the parsed definition in the cache
				 */
				phalcon_orm_set_prepared_ast(unique_id, *result TSRMLS_CC);

			} else {
				efree(parser_status->ret);
			}
		}
	}

	zval_ptr_dtor(&unique_id);

	efree(parser_status);
	efree(state);

	return status;
}
