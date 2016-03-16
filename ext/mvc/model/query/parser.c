/* Driver template for the LEMON parser generator.
** The author disclaims copyright to this source code.
*/
/* First off, code is include which follows the "include" declaration
** in the input file. */
#include <stdio.h>
/* #line 40 "parser.y" */


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


/* #line 478 "parser.c" */
/* Next is all token values, in a form suitable for use by makeheaders.
** This section will be null unless lemon is run with the -m switch.
*/
/* 
** These constants (all generated automatically by the parser generator)
** specify the various kinds of tokens (terminals) that the parser
** understands. 
**
** Each symbol here is a terminal symbol in the grammar.
*/
/* Make sure the INTERFACE macro is defined.
*/
#ifndef INTERFACE
# define INTERFACE 1
#endif
/* The next thing included is series of defines which control
** various aspects of the generated parser.
**    YYCODETYPE         is the data type used for storing terminal
**                       and nonterminal numbers.  "unsigned char" is
**                       used if there are fewer than 250 terminals
**                       and nonterminals.  "int" is used otherwise.
**    YYNOCODE           is a number of type YYCODETYPE which corresponds
**                       to no legal terminal or nonterminal number.  This
**                       number is used to fill in empty slots of the hash 
**                       table.
**    YYFALLBACK         If defined, this indicates that one or more tokens
**                       have fall-back values which should be used if the
**                       original value of the token will not parse.
**    YYACTIONTYPE       is the data type used for storing terminal
**                       and nonterminal numbers.  "unsigned char" is
**                       used if there are fewer than 250 rules and
**                       states combined.  "int" is used otherwise.
**    phql_TOKENTYPE     is the data type used for minor tokens given 
**                       directly to the parser from the tokenizer.
**    YYMINORTYPE        is the data type used for all minor tokens.
**                       This is typically a union of many types, one of
**                       which is phql_TOKENTYPE.  The entry in the union
**                       for base tokens is called "yy0".
**    YYSTACKDEPTH       is the maximum depth of the parser's stack.
**    phql_ARG_SDECL     A static variable declaration for the %extra_argument
**    phql_ARG_PDECL     A parameter declaration for the %extra_argument
**    phql_ARG_STORE     Code to store %extra_argument into yypParser
**    phql_ARG_FETCH     Code to extract %extra_argument from yypParser
**    YYNSTATE           the combined number of states.
**    YYNRULE            the number of rules in the grammar
**    YYERRORSYMBOL      is the code number of the error symbol.  If not
**                       defined, then do no error processing.
*/
#define YYCODETYPE unsigned char
#define YYNOCODE 141
#define YYACTIONTYPE unsigned short int
#define phql_TOKENTYPE phql_parser_token*
typedef union {
  phql_TOKENTYPE yy0;
  zval* yy8;
  int yy281;
} YYMINORTYPE;
#define YYSTACKDEPTH 100
#define phql_ARG_SDECL phql_parser_status *status;
#define phql_ARG_PDECL ,phql_parser_status *status
#define phql_ARG_FETCH phql_parser_status *status = yypParser->status
#define phql_ARG_STORE yypParser->status = status
#define YYNSTATE 301
#define YYNRULE 163
#define YYERRORSYMBOL 87
#define YYERRSYMDT yy281
#define YY_NO_ACTION      (YYNSTATE+YYNRULE+2)
#define YY_ACCEPT_ACTION  (YYNSTATE+YYNRULE+1)
#define YY_ERROR_ACTION   (YYNSTATE+YYNRULE)

/* Next are that tables used to determine what action to take based on the
** current state and lookahead token.  These tables are used to implement
** functions that take a state number and lookahead value and return an
** action integer.  
**
** Suppose the action integer is N.  Then the action is determined as
** follows
**
**   0 <= N < YYNSTATE                  Shift N.  That is, push the lookahead
**                                      token onto the stack and goto state N.
**
**   YYNSTATE <= N < YYNSTATE+YYNRULE   Reduce by rule N-YYNSTATE.
**
**   N == YYNSTATE+YYNRULE              A syntax error has occurred.
**
**   N == YYNSTATE+YYNRULE+1            The parser accepts its input.
**
**   N == YYNSTATE+YYNRULE+2            No such action.  Denotes unused
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
*/
static YYACTIONTYPE yy_action[] = {
 /*     0 */    92,   98,   32,   34,   36,   38,   40,   54,   42,   44,
 /*    10 */    46,   48,   50,   52,   22,   24,   56,   61,   26,   28,
 /*    20 */    30,   18,   16,   20,   14,   11,   94,   63,   58,   32,
 /*    30 */    34,   36,   38,   40,   54,   42,   44,   46,   48,   50,
 /*    40 */    52,   22,   24,   56,   61,   26,   28,   30,   18,   16,
 /*    50 */    20,   14,   11,   94,   63,   58,  301,   90,   91,   92,
 /*    60 */    98,   32,   34,   36,   38,   40,   54,   42,   44,   46,
 /*    70 */    48,   50,   52,   22,   24,   56,   61,   26,   28,   30,
 /*    80 */    18,   16,   20,   14,   11,   94,   63,   58,   92,   98,
 /*    90 */    32,   34,   36,   38,   40,   54,   42,   44,   46,   48,
 /*   100 */    50,   52,   22,   24,   56,   61,   26,   28,   30,   18,
 /*   110 */    16,   20,   14,   11,   94,   63,   58,   78,   79,   80,
 /*   120 */    81,  106,  203,  225,  112,  223,  108,  202,  110,  111,
 /*   130 */    94,   63,   58,  138,  302,  177,  183,   92,   98,   32,
 /*   140 */    34,   36,   38,   40,   54,   42,   44,   46,   48,   50,
 /*   150 */    52,   22,   24,   56,   61,   26,   28,   30,   18,   16,
 /*   160 */    20,   14,   11,   94,   63,   58,   18,   16,   20,   14,
 /*   170 */    11,   94,   63,   58,  124,    5,   92,   98,   32,   34,
 /*   180 */    36,   38,   40,   54,   42,   44,   46,   48,   50,   52,
 /*   190 */    22,   24,   56,   61,   26,   28,   30,   18,   16,   20,
 /*   200 */    14,   11,   94,   63,   58,   92,   98,   32,   34,   36,
 /*   210 */    38,   40,   54,   42,   44,   46,   48,   50,   52,   22,
 /*   220 */    24,   56,   61,   26,   28,   30,   18,   16,   20,   14,
 /*   230 */    11,   94,   63,   58,   56,   61,   26,   28,   30,   18,
 /*   240 */    16,   20,   14,   11,   94,   63,   58,  130,  175,   92,
 /*   250 */    98,   32,   34,   36,   38,   40,   54,   42,   44,   46,
 /*   260 */    48,   50,   52,   22,   24,   56,   61,   26,   28,   30,
 /*   270 */    18,   16,   20,   14,   11,   94,   63,   58,  227,  228,
 /*   280 */   140,   22,   24,   56,   61,   26,   28,   30,   18,   16,
 /*   290 */    20,   14,   11,   94,   63,   58,  230,  171,   92,   98,
 /*   300 */    32,   34,   36,   38,   40,   54,   42,   44,   46,   48,
 /*   310 */    50,   52,   22,   24,   56,   61,   26,   28,   30,   18,
 /*   320 */    16,   20,   14,   11,   94,   63,   58,  152,   59,  249,
 /*   330 */    12,  175,  146,  153,  155,  257,  101,   89,  268,  251,
 /*   340 */   143,  144,  264,  465,    1,    2,    3,  259,  260,   65,
 /*   350 */   157,  100,  119,  157,  152,  157,   65,   12,  149,   88,
 /*   360 */   153,  155,  147,  142,  169,   58,  142,  143,  142,  158,
 /*   370 */   159,  165,  166,  117,  121,  127,  279,  133,  100,  437,
 /*   380 */   171,   64,  295,  162,  160,  161,  163,  164,  167,  168,
 /*   390 */    14,   11,   94,   63,   58,  243,  158,  159,  165,  166,
 /*   400 */   117,  121,  127,   66,  133,  109,  110,  111,  135,  179,
 /*   410 */   162,  160,  161,  163,  164,  167,  168,  216,  236,  104,
 /*   420 */    42,   44,   46,   48,   50,   52,   22,   24,   56,   61,
 /*   430 */    26,   28,   30,   18,   16,   20,   14,   11,   94,   63,
 /*   440 */    58,   12,  146,   76,  153,  155,   67,  101,  287,  294,
 /*   450 */   290,  143,  185,  201,  187,  189,  234,  193,  197,  146,
 /*   460 */   157,    8,  100,  218,  136,  138,   12,  177,  149,  153,
 /*   470 */   155,   89,  213,  142,  169,   82,  219,  157,    7,   68,
 /*   480 */   158,  159,  165,  166,  117,  121,  127,  100,  133,  157,
 /*   490 */   142,  151,   86,  233,  162,  160,  161,  163,  164,  167,
 /*   500 */   168,  182,  142,  180,  263,  158,  159,  165,  166,  117,
 /*   510 */   121,  127,  192,  133,  303,  190,  183,   84,  220,  162,
 /*   520 */   160,  161,  163,  164,  167,  168,  221,  101,  144,  152,
 /*   530 */   241,   69,   12,  231,   10,  153,  155,   65,  196,  176,
 /*   540 */    74,  194,  143,    6,  261,    4,  258,  284,  157,  298,
 /*   550 */   229,  107,  157,  100,  240,  108,  202,  110,  111,   12,
 /*   560 */    96,  142,  153,  155,   72,  142,   10,  171,  157,  143,
 /*   570 */   383,  158,  159,  165,  166,  117,  121,  127,    9,  133,
 /*   580 */   100,  142,  289,  290,  157,  162,  160,  161,  163,  164,
 /*   590 */   167,  168,  150,  209,   71,  207,   70,  142,  158,  159,
 /*   600 */   165,  166,  117,  121,  127,   75,  133,   73,  200,  436,
 /*   610 */    95,  198,  162,  160,  161,  163,  164,  167,  168,   26,
 /*   620 */    28,   30,   18,   16,   20,   14,   11,   94,   63,   58,
 /*   630 */   204,   13,  282,  247,   77,  150,  235,   65,  185,  201,
 /*   640 */   187,  189,  269,  193,  197,  278,  205,   83,  150,  157,
 /*   650 */   146,  206,  246,  255,  157,  103,  293,   65,   85,  226,
 /*   660 */   222,  241,  142,  183,   87,  254,  285,  142,  157,    8,
 /*   670 */   146,  206,   97,  275,  157,  292,  245,  210,  157,  157,
 /*   680 */   102,  142,  169,  183,  238,  242,  267,  142,  157,  184,
 /*   690 */   270,  142,  142,   15,   10,   10,  253,  217,  222,   17,
 /*   700 */   115,  142,  169,  266,  273,  105,  258,  258,  113,  120,
 /*   710 */   215,  157,  157,  157,   19,  206,  157,  157,    8,  118,
 /*   720 */    21,   23,   25,   27,  142,  142,  142,  183,   29,  142,
 /*   730 */   142,   31,  157,  114,  125,  274,  101,   33,  157,  157,
 /*   740 */   157,  157,  122,  131,  126,  142,  157,   35,   37,  157,
 /*   750 */   128,  142,  142,  142,  142,  157,  132,   39,   41,  142,
 /*   760 */   137,  148,  142,   43,   45,  157,  157,   47,  142,   49,
 /*   770 */   170,  145,  173,  172,  186,  157,  157,  174,  142,  142,
 /*   780 */   176,  157,  157,   51,   53,  157,   55,  157,  142,  142,
 /*   790 */    57,  181,   60,   62,  142,  142,  330,  188,  142,  331,
 /*   800 */   142,  157,  157,  332,  157,  191,  333,   93,  157,   99,
 /*   810 */   157,  157,  116,  123,  142,  142,  195,  142,  129,  134,
 /*   820 */   334,  142,  335,  142,  142,  157,  336,  157,  139,  141,
 /*   830 */   157,  157,  199,  154,  156,  178,  157,  157,  142,  244,
 /*   840 */   142,  337,  338,  142,  142,  250,  157,  157,  208,  142,
 /*   850 */   142,  157,  157,  157,  211,  212,  300,  157,  214,  142,
 /*   860 */   142,  206,  224,  157,  142,  142,  142,  232,  237,  239,
 /*   870 */   142,  248,  252,  183,  304,  256,  142,  305,  262,  265,
 /*   880 */   277,  271,  272,  280,  276,  350,  283,  382,  281,  286,
 /*   890 */   288,  291,  296,  297,  356,  299,
};
static YYCODETYPE yy_lookahead[] = {
 /*     0 */     1,    2,    3,    4,    5,    6,    7,    8,    9,   10,
 /*    10 */    11,   12,   13,   14,   15,   16,   17,   18,   19,   20,
 /*    20 */    21,   22,   23,   24,   25,   26,   27,   28,   29,    3,
 /*    30 */     4,    5,    6,    7,    8,    9,   10,   11,   12,   13,
 /*    40 */    14,   15,   16,   17,   18,   19,   20,   21,   22,   23,
 /*    50 */    24,   25,   26,   27,   28,   29,    0,   58,   59,    1,
 /*    60 */     2,    3,    4,    5,    6,    7,    8,    9,   10,   11,
 /*    70 */    12,   13,   14,   15,   16,   17,   18,   19,   20,   21,
 /*    80 */    22,   23,   24,   25,   26,   27,   28,   29,    1,    2,
 /*    90 */     3,    4,    5,    6,    7,    8,    9,   10,   11,   12,
 /*   100 */    13,   14,   15,   16,   17,   18,   19,   20,   21,   22,
 /*   110 */    23,   24,   25,   26,   27,   28,   29,   65,   66,   67,
 /*   120 */    68,  104,  105,   36,  113,   38,  109,  110,  111,  112,
 /*   130 */    27,   28,   29,   75,    0,   77,  125,    1,    2,    3,
 /*   140 */     4,    5,    6,    7,    8,    9,   10,   11,   12,   13,
 /*   150 */    14,   15,   16,   17,   18,   19,   20,   21,   22,   23,
 /*   160 */    24,   25,   26,   27,   28,   29,   22,   23,   24,   25,
 /*   170 */    26,   27,   28,   29,   38,   47,    1,    2,    3,    4,
 /*   180 */     5,    6,    7,    8,    9,   10,   11,   12,   13,   14,
 /*   190 */    15,   16,   17,   18,   19,   20,   21,   22,   23,   24,
 /*   200 */    25,   26,   27,   28,   29,    1,    2,    3,    4,    5,
 /*   210 */     6,    7,    8,    9,   10,   11,   12,   13,   14,   15,
 /*   220 */    16,   17,   18,   19,   20,   21,   22,   23,   24,   25,
 /*   230 */    26,   27,   28,   29,   17,   18,   19,   20,   21,   22,
 /*   240 */    23,   24,   25,   26,   27,   28,   29,   72,   37,    1,
 /*   250 */     2,    3,    4,    5,    6,    7,    8,    9,   10,   11,
 /*   260 */    12,   13,   14,   15,   16,   17,   18,   19,   20,   21,
 /*   270 */    22,   23,   24,   25,   26,   27,   28,   29,   34,   35,
 /*   280 */    76,   15,   16,   17,   18,   19,   20,   21,   22,   23,
 /*   290 */    24,   25,   26,   27,   28,   29,   48,   86,    1,    2,
 /*   300 */     3,    4,    5,    6,    7,    8,    9,   10,   11,   12,
 /*   310 */    13,   14,   15,   16,   17,   18,   19,   20,   21,   22,
 /*   320 */    23,   24,   25,   26,   27,   28,   29,   23,   17,   18,
 /*   330 */    26,   37,  107,   29,   30,  107,   32,  107,   47,   28,
 /*   340 */    36,   47,   51,   88,   89,   90,   91,   92,   93,   94,
 /*   350 */   125,   47,   90,  125,   23,  125,   94,   26,  133,  129,
 /*   360 */    29,   30,  137,  138,  139,   29,  138,   36,  138,   65,
 /*   370 */    66,   67,   68,   69,   70,   71,  121,   73,   47,   48,
 /*   380 */    86,   47,  127,   79,   80,   81,   82,   83,   84,   85,
 /*   390 */    25,   26,   27,   28,   29,   55,   65,   66,   67,   68,
 /*   400 */    69,   70,   71,   95,   73,  110,  111,  112,  134,  135,
 /*   410 */    79,   80,   81,   82,   83,   84,   85,   31,   60,   33,
 /*   420 */     9,   10,   11,   12,   13,   14,   15,   16,   17,   18,
 /*   430 */    19,   20,   21,   22,   23,   24,   25,   26,   27,   28,
 /*   440 */    29,   26,  107,   31,   29,   30,   96,   32,  123,  124,
 /*   450 */   125,   36,   39,   40,   41,   42,   61,   44,   45,  107,
 /*   460 */   125,   31,   47,   23,   74,   75,   26,   77,  133,   29,
 /*   470 */    30,  107,  137,  138,  139,   63,   36,  125,   48,   97,
 /*   480 */    65,   66,   67,   68,   69,   70,   71,   47,   73,  125,
 /*   490 */   138,  139,  128,  129,   79,   80,   81,   82,   83,   84,
 /*   500 */    85,   36,  138,   38,  113,   65,   66,   67,   68,   69,
 /*   510 */    70,   71,   40,   73,    0,   43,  125,   56,   37,   79,
 /*   520 */    80,   81,   82,   83,   84,   85,   23,   32,   47,   23,
 /*   530 */   107,   98,   26,   90,  107,   29,   30,   94,   40,   36,
 /*   540 */    62,   43,   36,  116,   49,   31,  119,   52,  125,   54,
 /*   550 */   107,  105,  125,   47,  131,  109,  110,  111,  112,   26,
 /*   560 */    29,  138,   29,   30,   64,  138,  107,   86,  125,   36,
 /*   570 */     0,   65,   66,   67,   68,   69,   70,   71,  119,   73,
 /*   580 */    47,  138,  124,  125,  125,   79,   80,   81,   82,   83,
 /*   590 */    84,   85,   31,   36,  100,   38,   99,  138,   65,   66,
 /*   600 */    67,   68,   69,   70,   71,  132,   73,   52,   40,   48,
 /*   610 */    79,   43,   79,   80,   81,   82,   83,   84,   85,   19,
 /*   620 */    20,   21,   22,   23,   24,   25,   26,   27,   28,   29,
 /*   630 */    31,  107,   62,   90,  132,   31,  107,   94,   39,   40,
 /*   640 */    41,   42,  117,   44,   45,  120,  108,  132,   31,  125,
 /*   650 */   107,  113,   48,   90,  125,  102,  107,   94,   57,  106,
 /*   660 */   107,  107,  138,  125,   31,   48,  108,  138,  125,   31,
 /*   670 */   107,  113,   79,   31,  125,  126,  133,   78,  125,  125,
 /*   680 */   101,  138,  139,  125,  130,  131,   48,  138,  125,   36,
 /*   690 */    48,  138,  138,  107,  107,  107,  133,  106,  107,  107,
 /*   700 */    46,  138,  139,  116,  116,  103,  119,  119,  114,   48,
 /*   710 */   108,  125,  125,  125,  107,  113,  125,  125,   31,   47,
 /*   720 */   107,  107,  107,  107,  138,  138,  138,  125,  107,  138,
 /*   730 */   138,  107,  125,  115,   36,   48,   32,  107,  125,  125,
 /*   740 */   125,  125,   47,   36,   48,  138,  125,  107,  107,  125,
 /*   750 */    47,  138,  138,  138,  138,  125,   48,  107,  107,  138,
 /*   760 */   135,   48,  138,  107,  107,  125,  125,  107,  138,  107,
 /*   770 */    34,  136,   37,   36,   40,  125,  125,   36,  138,  138,
 /*   780 */    36,  125,  125,  107,  107,  125,  107,  125,  138,  138,
 /*   790 */   107,   36,  107,  107,  138,  138,   36,   40,  138,   36,
 /*   800 */   138,  125,  125,   36,  125,   40,   36,  107,  125,  107,
 /*   810 */   125,  125,  107,  107,  138,  138,   40,  138,  107,  107,
 /*   820 */    36,  138,   36,  138,  138,  125,   36,  125,  107,  107,
 /*   830 */   125,  125,   40,  107,  107,  107,  125,  125,  138,  107,
 /*   840 */   138,   36,   36,  138,  138,  107,  125,  125,   36,  138,
 /*   850 */   138,  125,  125,  125,   47,  136,  108,  125,   48,  138,
 /*   860 */   138,  113,   36,  125,  138,  138,  138,   48,   57,   31,
 /*   870 */   138,   48,   47,  125,    0,   48,  138,    0,   50,   47,
 /*   880 */    36,   51,   47,   95,  120,    0,  132,    0,  122,   53,
 /*   890 */    31,    3,   95,  122,    0,   33,
};
#define YY_SHIFT_USE_DFLT (-2)
static short yy_shift_ofst[] = {
 /*     0 */   495,   56,  134,  514,  128,  533,  430,   -2,  533,   -2,
 /*    10 */   297,  533,  533,  103,  533,  103,  533,  365,  533,  365,
 /*    20 */   533,  365,  533,  217,  533,  217,  533,  144,  533,  144,
 /*    30 */   533,  144,  533,  411,  533,  411,  533,  411,  533,  411,
 /*    40 */   533,  411,  533,  266,  533,  266,  533,  266,  533,  266,
 /*    50 */   533,  266,  533,  266,  533,  411,  533,  600,  311,  533,
 /*    60 */   336,  533,  600,  334,  304,  340,  358,  395,  461,  478,
 /*    70 */   500,   -2,  555,   -2,   52,  412,   52,   -2,   -2,   -2,
 /*    80 */    -2,   -2,   52,   -2,  601,  533,  633,  533,   -2,   -1,
 /*    90 */    -2,   -2,  533,  297,  531,   -2,  593,   -2,  533,   26,
 /*   100 */   415,  244,  440,  386,  653,  599,  413,   -2,  413,   -2,
 /*   110 */    -2,  653,  465,  654,   -2,  533,  297,  672,  704,  661,
 /*   120 */    -2,  695,  533,  136,  698,  696,   -2,  703,  533,  175,
 /*   130 */   707,  708,   -2,  533,   58,  390,   -2,   -2,  533,  204,
 /*   140 */   533,  297,   -2,  294,  736,  331,  297,  713,   -2,  561,
 /*   150 */   506,   -2,   -2,  533,  336,  533,  336,   -2,   -2,   -2,
 /*   160 */    -2,   -2,   -2,   -2,   -2,   -2,   -2,   -2,   -2,   -2,
 /*   170 */    -2,  737,  735,  741,   -2,  744,   -2,  533,  297,   -2,
 /*   180 */   755,   -2,   -2,   -2,  211,  734,  760,  757,  763,  472,
 /*   190 */   765,  767,  770,  498,  776,  784,  786,  568,  792,  790,
 /*   200 */   805,  806,   -2,   -2,  653,   -2,  557,  812,   -2,   -2,
 /*   210 */   807,  736,  331,  810,   -2,   -2,  440,   -2,   -2,  481,
 /*   220 */   503,   -2,   87,  826,   -2,   -2,   -2,   -2,   -2,  248,
 /*   230 */    -2,  819,   -2,   -2,  533,  297,  811,  533,  838,  533,
 /*   240 */    -2,  297,   -2,  533,  297,  604,   -2,  823,   -2,  533,
 /*   250 */   336,  825,  304,  617,   -2,  827,   -2,  103,   -2,  874,
 /*   260 */   877,  828,  653,  291,  832,  533,  638,   -2,  844,  642,
 /*   270 */   830,  835,  533,  687,   -2,  844,   -2,   -2,   -2,  340,
 /*   280 */   570,  885,   52,  887,  653,  836,  653,  859,  653,   -2,
 /*   290 */   888,  533,   -2,  297,   -2,  340,  570,  894,  862,  653,
 /*   300 */    -2,
};
#define YY_REDUCE_USE_DFLT (-1)
static short yy_reduce_ofst[] = {
 /*     0 */   255,   -1,   -1,   -1,   -1,  427,   -1,   -1,  459,   -1,
 /*    10 */    -1,  228,  524,   -1,  586,   -1,  592,   -1,  607,   -1,
 /*    20 */   613,   -1,  614,   -1,  615,   -1,  616,   -1,  621,   -1,
 /*    30 */   624,   -1,  630,   -1,  640,   -1,  641,   -1,  650,   -1,
 /*    40 */   651,   -1,  656,   -1,  657,   -1,  660,   -1,  662,   -1,
 /*    50 */   676,   -1,  677,   -1,  679,   -1,  683,   -1,   -1,  685,
 /*    60 */    -1,  686,   -1,   -1,  543,  308,  350,  382,  433,  497,
 /*    70 */   494,   -1,   -1,   -1,  473,   -1,  502,   -1,   -1,   -1,
 /*    80 */    -1,   -1,  515,   -1,   -1,  364,   -1,  230,   -1,   -1,
 /*    90 */    -1,   -1,  700,   -1,   -1,   -1,   -1,   -1,  702,   -1,
 /*   100 */   443,  579,  553,   -1,  602,   17,  446,   -1,  295,   -1,
 /*   110 */    -1,   11,  594,  618,   -1,  705,   -1,   -1,  262,   -1,
 /*   120 */    -1,   -1,  706,   -1,   -1,   -1,   -1,   -1,  711,   -1,
 /*   130 */    -1,   -1,   -1,  712,  274,  625,   -1,   -1,  721,   -1,
 /*   140 */   722,   -1,   -1,   -1,  635,  225,   -1,   -1,   -1,   -1,
 /*   150 */   352,   -1,   -1,  726,   -1,  727,   -1,   -1,   -1,   -1,
 /*   160 */    -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
 /*   170 */    -1,   -1,   -1,   -1,   -1,   -1,   -1,  728,   -1,   -1,
 /*   180 */    -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
 /*   190 */    -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
 /*   200 */    -1,   -1,   -1,   -1,  538,   -1,   -1,   -1,   -1,   -1,
 /*   210 */    -1,  719,  335,   -1,   -1,   -1,  591,   -1,   -1,   -1,
 /*   220 */    -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
 /*   230 */    -1,   -1,   -1,   -1,  529,   -1,   -1,  554,   -1,  423,
 /*   240 */    -1,   -1,   -1,  732,   -1,   -1,   -1,   -1,   -1,  738,
 /*   250 */    -1,   -1,  563,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
 /*   260 */    -1,   -1,  391,   -1,   -1,  587,   -1,   -1,  525,   -1,
 /*   270 */    -1,   -1,  588,   -1,   -1,  764,   -1,   -1,   -1,  788,
 /*   280 */   766,   -1,  754,   -1,  558,   -1,  325,   -1,  458,   -1,
 /*   290 */    -1,  549,   -1,   -1,   -1,  797,  771,   -1,   -1,  748,
 /*   300 */    -1,
};
static YYACTIONTYPE yy_default[] = {
 /*     0 */   464,  464,  464,  464,  464,  464,  464,  341,  464,  344,
 /*    10 */   346,  464,  464,  390,  464,  392,  464,  393,  464,  394,
 /*    20 */   464,  395,  464,  396,  464,  397,  464,  398,  464,  399,
 /*    30 */   464,  400,  464,  401,  464,  402,  464,  403,  464,  404,
 /*    40 */   464,  405,  464,  406,  464,  407,  464,  408,  464,  409,
 /*    50 */   464,  410,  464,  411,  464,  412,  464,  413,  464,  464,
 /*    60 */   414,  464,  415,  464,  464,  363,  372,  377,  365,  381,
 /*    70 */   385,  306,  464,  384,  464,  378,  464,  379,  386,  387,
 /*    80 */   388,  389,  464,  380,  464,  464,  364,  464,  366,  368,
 /*    90 */   369,  370,  464,  423,  464,  442,  464,  443,  464,  444,
 /*   100 */   464,  311,  464,  464,  464,  322,  322,  307,  321,  323,
 /*   110 */   325,  464,  329,  340,  326,  464,  339,  464,  464,  464,
 /*   120 */   422,  464,  464,  464,  464,  464,  424,  464,  464,  464,
 /*   130 */   464,  464,  425,  464,  464,  464,  426,  427,  464,  464,
 /*   140 */   464,  429,  432,  463,  435,  464,  441,  464,  433,  464,
 /*   150 */   464,  438,  440,  464,  445,  464,  446,  448,  449,  450,
 /*   160 */   451,  452,  453,  454,  455,  456,  457,  458,  459,  439,
 /*   170 */   434,  464,  461,  464,  460,  464,  462,  464,  430,  428,
 /*   180 */   464,  327,  328,  361,  463,  464,  464,  464,  464,  464,
 /*   190 */   464,  464,  464,  464,  464,  464,  464,  464,  464,  464,
 /*   200 */   464,  464,  324,  308,  464,  319,  360,  464,  358,  359,
 /*   210 */   464,  435,  464,  464,  431,  320,  464,  312,  314,  463,
 /*   220 */   464,  315,  318,  464,  316,  317,  313,  309,  310,  464,
 /*   230 */   447,  464,  419,  367,  464,  376,  464,  464,  371,  464,
 /*   240 */   373,  375,  374,  464,  362,  464,  417,  464,  420,  464,
 /*   250 */   416,  464,  464,  464,  418,  464,  421,  391,  345,  464,
 /*   260 */   464,  464,  464,  464,  464,  464,  464,  342,  464,  464,
 /*   270 */   464,  464,  464,  464,  343,  464,  347,  349,  348,  363,
 /*   280 */   464,  464,  464,  464,  464,  464,  464,  351,  464,  352,
 /*   290 */   464,  464,  354,  355,  353,  363,  464,  464,  464,  464,
 /*   300 */   357,
};
#define YY_SZ_ACTTAB (sizeof(yy_action)/sizeof(yy_action[0]))

/* The next table maps tokens into fallback tokens.  If a construct
** like the following:
** 
**      %fallback ID X Y Z.
**
** appears in the grammer, then ID becomes a fallback token for X, Y,
** and Z.  Whenever one of the tokens X, Y, or Z is input to the parser
** but it does not parse, the type of the token is changed to ID and
** the parse is retried before an error is thrown.
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
*/
struct yyStackEntry {
  int stateno;       /* The state-number */
  int major;         /* The major token value.  This is the code
                     ** number for the token at this stack level */
  YYMINORTYPE minor; /* The user-supplied minor token value.  This
                     ** is the value of the token  */
};
typedef struct yyStackEntry yyStackEntry;

/* The state of the parser is completely contained in an instance of
** the following structure */
struct yyParser {
  int yyidx;                    /* Index of top element in stack */
  int yyerrcnt;                 /* Shifts left before out of the error */
  phql_ARG_SDECL                /* A place to hold %extra_argument */
  yyStackEntry yystack[YYSTACKDEPTH];  /* The parser's stack */
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
static const char *yyTokenName[] = { 
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
  "values_list",   "field_list",    "value_list",    "value_item",  
  "field_item",    "update_clause",  "limit_clause",  "update_item_list",
  "update_item",   "qualified_name",  "new_value",     "delete_clause",
  "order_list",    "order_item",    "group_list",    "group_item",  
  "integer_or_placeholder",  "argument_list",  "when_clauses",  "when_clause", 
  "distinct_or_null",  "argument_list_or_null",  "function_call",  "argument_item",
};
#endif /* NDEBUG */

#ifndef NDEBUG
/* For tracing reduce actions, the names of all rules are required.
*/
static const char *yyRuleName[] = {
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

/*
** This function returns the symbolic name associated with a token
** value.
*/
const char *phql_TokenName(int tokenType){
#ifndef NDEBUG
  if( tokenType>0 && (size_t)tokenType<(sizeof(yyTokenName)/sizeof(yyTokenName[0])) ){
    return yyTokenName[tokenType];
  }else{
    return "Unknown";
  }
#else
  return "";
#endif
}

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
void *phql_Alloc(void *(*mallocProc)(size_t)){
  yyParser *pParser;
  pParser = (yyParser*)(*mallocProc)( (size_t)sizeof(yyParser) );
  if( pParser ){
    pParser->yyidx = -1;
  }
  return pParser;
}

/* The following function deletes the value associated with a
** symbol.  The symbol can be either a terminal or nonterminal.
** "yymajor" is the symbol code, and "yypminor" is a pointer to
** the value.
*/
static void yy_destructor(YYCODETYPE yymajor, YYMINORTYPE *yypminor){
  switch( yymajor ){
    /* Here is inserted the actions which take place when a
    ** terminal or non-terminal is destroyed.  This can happen
    ** when the symbol is popped from the stack during a
    ** reduce or during error processing or when a parser is 
    ** being destroyed before it is finished parsing.
    **
    ** Note: during a reduce, the only symbols destroyed are those
    ** which appear on the RHS of the rule, but which are not used
    ** inside the C code.
    */
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
    case 19:
    case 20:
    case 21:
    case 22:
    case 23:
    case 24:
    case 25:
    case 26:
    case 27:
    case 28:
    case 29:
    case 30:
    case 31:
    case 32:
    case 33:
    case 34:
    case 35:
    case 36:
    case 37:
    case 38:
    case 39:
    case 40:
    case 41:
    case 42:
    case 43:
    case 44:
    case 45:
    case 46:
    case 47:
    case 48:
    case 49:
    case 50:
    case 51:
    case 52:
    case 53:
    case 54:
    case 55:
    case 56:
    case 57:
    case 58:
    case 59:
    case 60:
    case 61:
    case 62:
    case 63:
    case 64:
    case 65:
    case 66:
    case 67:
    case 68:
    case 69:
    case 70:
    case 71:
    case 72:
    case 73:
    case 74:
    case 75:
    case 76:
    case 77:
    case 78:
    case 79:
    case 80:
    case 81:
    case 82:
    case 83:
    case 84:
    case 85:
    case 86:
/* #line 577 "parser.y" */
{
	if ((yypminor->yy0)) {
		if ((yypminor->yy0)->free_flag) {
			efree((yypminor->yy0)->token);
		}
		efree((yypminor->yy0));
	}
}
/* #line 1322 "parser.c" */
      break;
    case 89:
    case 90:
    case 91:
    case 92:
    case 93:
    case 94:
    case 102:
    case 103:
    case 104:
    case 106:
    case 107:
    case 108:
    case 109:
    case 110:
    case 111:
    case 112:
    case 113:
    case 117:
    case 118:
    case 120:
    case 121:
    case 123:
    case 124:
    case 125:
    case 127:
    case 128:
    case 129:
    case 130:
    case 131:
    case 133:
    case 138:
    case 139:
/* #line 590 "parser.y" */
{ zval_ptr_dtor(&(yypminor->yy8)); }
/* #line 1358 "parser.c" */
      break;
    case 95:
    case 96:
    case 97:
    case 98:
    case 99:
    case 100:
    case 101:
    case 114:
    case 115:
    case 122:
    case 136:
    case 137:
/* #line 886 "parser.y" */
{ phalcon_safe_zval_ptr_dtor((yypminor->yy8)); }
/* #line 1374 "parser.c" */
      break;
    default:  break;   /* If no destructor action specified: do nothing */
  }
}

/*
** Pop the parser's stack once.
**
** If there is a destructor routine associated with the token which
** is popped from the stack, then call it.
**
** Return the major token number for the symbol popped.
*/
static int yy_pop_parser_stack(yyParser *pParser){
  YYCODETYPE yymajor;
  yyStackEntry *yytos = &pParser->yystack[pParser->yyidx];

  if( pParser->yyidx<0 ) return 0;
#ifndef NDEBUG
  if( yyTraceFILE && pParser->yyidx>=0 ){
    fprintf(yyTraceFILE,"%sPopping %s\n",
      yyTracePrompt,
      yyTokenName[yytos->major]);
  }
#endif
  yymajor = yytos->major;
  yy_destructor( yymajor, &yytos->minor);
  pParser->yyidx--;
  return yymajor;
}

/* 
** Deallocate and destroy a parser.  Destructors are all called for
** all stack elements before shutting the parser down.
**
** Inputs:
** <ul>
** <li>  A pointer to the parser.  This should be a pointer
**       obtained from phql_Alloc.
** <li>  A pointer to a function used to reclaim memory obtained
**       from malloc.
** </ul>
*/
void phql_Free(
  void *p,                    /* The parser to be deleted */
  void (*freeProc)(void*)     /* Function used to reclaim memory */
){
  yyParser *pParser = (yyParser*)p;
  if( pParser==0 ) return;
  while( pParser->yyidx>=0 ) yy_pop_parser_stack(pParser);
  (*freeProc)((void*)pParser);
}

/*
** Find the appropriate action for a parser given the terminal
** look-ahead token iLookAhead.
**
** If the look-ahead token is YYNOCODE, then check to see if the action is
** independent of the look-ahead.  If it is, return the action, otherwise
** return YY_NO_ACTION.
*/
static int yy_find_shift_action(
  yyParser *pParser,        /* The parser */
  int iLookAhead            /* The look-ahead token */
){
  int i;
  int stateno = pParser->yystack[pParser->yyidx].stateno;
 
  /* if( pParser->yyidx<0 ) return YY_NO_ACTION;  */
  i = yy_shift_ofst[stateno];
  if( i==YY_SHIFT_USE_DFLT ){
    return yy_default[stateno];
  }
  if( iLookAhead==YYNOCODE ){
    return YY_NO_ACTION;
  }
  i += iLookAhead;
  if( i<0 || i>=(int)YY_SZ_ACTTAB || yy_lookahead[i]!=iLookAhead ){
#ifdef YYFALLBACK
    int iFallback;            /* Fallback token */
    if( iLookAhead<sizeof(yyFallback)/sizeof(yyFallback[0])
           && (iFallback = yyFallback[iLookAhead])!=0 ){
#ifndef NDEBUG
      if( yyTraceFILE ){
        fprintf(yyTraceFILE, "%sFALLBACK %s => %s\n",
           yyTracePrompt, yyTokenName[iLookAhead], yyTokenName[iFallback]);
      }
#endif
      return yy_find_shift_action(pParser, iFallback);
    }
#endif
    return yy_default[stateno];
  }else{
    return yy_action[i];
  }
}

/*
** Find the appropriate action for a parser given the non-terminal
** look-ahead token iLookAhead.
**
** If the look-ahead token is YYNOCODE, then check to see if the action is
** independent of the look-ahead.  If it is, return the action, otherwise
** return YY_NO_ACTION.
*/
static int yy_find_reduce_action(
  yyParser *pParser,        /* The parser */
  int iLookAhead            /* The look-ahead token */
){
  int i;
  int stateno = pParser->yystack[pParser->yyidx].stateno;
 
  i = yy_reduce_ofst[stateno];
  if( i==YY_REDUCE_USE_DFLT ){
    return yy_default[stateno];
  }
  if( iLookAhead==YYNOCODE ){
    return YY_NO_ACTION;
  }
  i += iLookAhead;
  if( i<0 || i>=(int)YY_SZ_ACTTAB || yy_lookahead[i]!=iLookAhead ){
    return yy_default[stateno];
  }else{
    return yy_action[i];
  }
}

/*
** Perform a shift action.
*/
static void yy_shift(
  yyParser *yypParser,          /* The parser to be shifted */
  int yyNewState,               /* The new state to shift in */
  int yyMajor,                  /* The major token to shift in */
  YYMINORTYPE *yypMinor         /* Pointer ot the minor token to shift in */
){
  yyStackEntry *yytos;
  yypParser->yyidx++;
  if( yypParser->yyidx>=YYSTACKDEPTH ){
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
     phql_ARG_STORE; /* Suppress warning about unused %extra_argument var */
     return;
  }
  yytos = &yypParser->yystack[yypParser->yyidx];
  yytos->stateno = yyNewState;
  yytos->major = yyMajor;
  yytos->minor = *yypMinor;
#ifndef NDEBUG
  if( yyTraceFILE && yypParser->yyidx>0 ){
    int i;
    fprintf(yyTraceFILE,"%sShift %d\n",yyTracePrompt,yyNewState);
    fprintf(yyTraceFILE,"%sStack:",yyTracePrompt);
    for(i=1; i<=yypParser->yyidx; i++)
      fprintf(yyTraceFILE," %s",yyTokenName[yypParser->yystack[i].major]);
    fprintf(yyTraceFILE,"\n");
  }
#endif
}

/* The following table contains information about every rule that
** is used during the reduce.
*/
static struct {
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
  { 119, 1 },
  { 117, 3 },
  { 117, 1 },
  { 120, 1 },
  { 92, 3 },
  { 121, 4 },
  { 123, 3 },
  { 123, 1 },
  { 124, 3 },
  { 126, 1 },
  { 93, 3 },
  { 127, 3 },
  { 108, 3 },
  { 108, 2 },
  { 108, 1 },
  { 113, 1 },
  { 95, 2 },
  { 95, 0 },
  { 98, 3 },
  { 98, 0 },
  { 128, 3 },
  { 128, 1 },
  { 129, 1 },
  { 129, 2 },
  { 129, 2 },
  { 96, 3 },
  { 96, 0 },
  { 130, 3 },
  { 130, 1 },
  { 131, 1 },
  { 97, 2 },
  { 97, 0 },
  { 99, 2 },
  { 99, 4 },
  { 99, 4 },
  { 99, 0 },
  { 122, 2 },
  { 122, 0 },
  { 100, 2 },
  { 100, 0 },
  { 132, 1 },
  { 132, 1 },
  { 132, 1 },
  { 132, 1 },
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
  { 134, 2 },
  { 134, 1 },
  { 135, 4 },
  { 135, 2 },
  { 104, 5 },
  { 107, 1 },
  { 138, 5 },
  { 136, 1 },
  { 136, 0 },
  { 137, 1 },
  { 137, 0 },
  { 133, 3 },
  { 133, 1 },
  { 139, 1 },
  { 139, 1 },
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
  { 125, 5 },
  { 125, 3 },
  { 125, 3 },
  { 125, 1 },
};

static void yy_accept(yyParser*);  /* Forward Declaration */

/*
** Perform a reduce action and the shift that must immediately
** follow the reduce.
*/
static void yy_reduce(
  yyParser *yypParser,         /* The parser */
  int yyruleno                 /* Number of the rule by which to reduce */
){
  int yygoto;                     /* The next state */
  int yyact;                      /* The next action */
  YYMINORTYPE yygotominor;        /* The LHS of the rule reduced */
  yyStackEntry *yymsp;            /* The top of the parser's stack */
  int yysize;                     /* Amount to pop the stack */
  phql_ARG_FETCH;
  yymsp = &yypParser->yystack[yypParser->yyidx];
#ifndef NDEBUG
  if( yyTraceFILE && yyruleno>=0 
        && yyruleno<(int)(sizeof(yyRuleName)/sizeof(yyRuleName[0])) ){
    fprintf(yyTraceFILE, "%sReduce [%s].\n", yyTracePrompt,
      yyRuleName[yyruleno]);
  }
#endif /* NDEBUG */

  switch( yyruleno ){
  /* Beginning here are the reduction cases.  A typical example
  ** follows:
  **   case 0:
  **  #line <lineno> <grammarfile>
  **     { ... }           // User supplied code
  **  #line <lineno> <thisfile>
  **     break;
  */
      case 0:
/* #line 586 "parser.y" */
{
	status->ret = yymsp[0].minor.yy8;
}
/* #line 1754 "parser.c" */
        break;
      case 1:
      case 2:
      case 3:
      case 4:
      case 19:
      case 20:
      case 23:
      case 24:
      case 45:
      case 52:
      case 54:
      case 60:
      case 66:
      case 73:
      case 74:
      case 131:
      case 135:
      case 140:
      case 147:
/* #line 592 "parser.y" */
{
	yygotominor.yy8 = yymsp[0].minor.yy8;
}
/* #line 1779 "parser.c" */
        break;
      case 5:
/* #line 610 "parser.y" */
{
	yygotominor.yy8 = phql_ret_select_statement(yymsp[-6].minor.yy8, yymsp[-5].minor.yy8, yymsp[-2].minor.yy8, yymsp[-4].minor.yy8, yymsp[-3].minor.yy8, yymsp[-1].minor.yy8, yymsp[0].minor.yy8);
}
/* #line 1786 "parser.c" */
        break;
      case 6:
/* #line 616 "parser.y" */
{
	yygotominor.yy8 = phql_ret_select_clause(yymsp[-5].minor.yy8, yymsp[-4].minor.yy8, yymsp[-2].minor.yy8, yymsp[0].minor.yy8, yymsp[-1].minor.yy8);
  yy_destructor(32,&yymsp[-6].minor);
  yy_destructor(33,&yymsp[-3].minor);
}
/* #line 1795 "parser.c" */
        break;
      case 7:
/* #line 620 "parser.y" */
{
	yygotominor.yy8 = phql_ret_select_clause(yymsp[-4].minor.yy8, yymsp[-3].minor.yy8, yymsp[-1].minor.yy8, yymsp[0].minor.yy8, NULL);
  yy_destructor(32,&yymsp[-5].minor);
  yy_destructor(33,&yymsp[-2].minor);
}
/* #line 1804 "parser.c" */
        break;
      case 8:
/* #line 626 "parser.y" */
{
	yygotominor.yy8 = phql_ret_distinct_all(1);
  yy_destructor(34,&yymsp[0].minor);
}
/* #line 1812 "parser.c" */
        break;
      case 9:
/* #line 630 "parser.y" */
{
	yygotominor.yy8 = phql_ret_distinct_all(0);
  yy_destructor(35,&yymsp[0].minor);
}
/* #line 1820 "parser.c" */
        break;
      case 10:
      case 21:
      case 28:
      case 39:
      case 62:
      case 64:
      case 71:
      case 76:
      case 80:
      case 82:
      case 84:
      case 134:
      case 136:
/* #line 634 "parser.y" */
{
	yygotominor.yy8 = NULL;
}
/* #line 1839 "parser.c" */
        break;
      case 11:
      case 18:
      case 43:
      case 46:
      case 51:
      case 65:
      case 72:
      case 137:
/* #line 640 "parser.y" */
{
	yygotominor.yy8 = phql_ret_zval_list(yymsp[-2].minor.yy8, yymsp[0].minor.yy8);
  yy_destructor(31,&yymsp[-1].minor);
}
/* #line 1854 "parser.c" */
        break;
      case 12:
      case 44:
      case 47:
      case 127:
      case 138:
/* #line 644 "parser.y" */
{
	yygotominor.yy8 = phql_ret_zval_list(yymsp[0].minor.yy8, NULL);
}
/* #line 1865 "parser.c" */
        break;
      case 13:
      case 139:
/* #line 650 "parser.y" */
{
	yygotominor.yy8 = phql_ret_column_item(PHQL_T_STARALL, NULL, NULL, NULL);
  yy_destructor(23,&yymsp[0].minor);
}
/* #line 1874 "parser.c" */
        break;
      case 14:
/* #line 654 "parser.y" */
{
	yygotominor.yy8 = phql_ret_column_item(PHQL_T_DOMAINALL, NULL, yymsp[-2].minor.yy0, NULL);
  yy_destructor(37,&yymsp[-1].minor);
  yy_destructor(23,&yymsp[0].minor);
}
/* #line 1883 "parser.c" */
        break;
      case 15:
/* #line 658 "parser.y" */
{
	yygotominor.yy8 = phql_ret_column_item(PHQL_T_EXPR, yymsp[-2].minor.yy8, NULL, yymsp[0].minor.yy0);
  yy_destructor(38,&yymsp[-1].minor);
}
/* #line 1891 "parser.c" */
        break;
      case 16:
/* #line 662 "parser.y" */
{
	yygotominor.yy8 = phql_ret_column_item(PHQL_T_EXPR, yymsp[-1].minor.yy8, NULL, yymsp[0].minor.yy0);
}
/* #line 1898 "parser.c" */
        break;
      case 17:
/* #line 666 "parser.y" */
{
	yygotominor.yy8 = phql_ret_column_item(PHQL_T_EXPR, yymsp[0].minor.yy8, NULL, NULL);
}
/* #line 1905 "parser.c" */
        break;
      case 22:
      case 126:
/* #line 690 "parser.y" */
{
	yygotominor.yy8 = phql_ret_zval_list(yymsp[-1].minor.yy8, yymsp[0].minor.yy8);
}
/* #line 1913 "parser.c" */
        break;
      case 25:
/* #line 707 "parser.y" */
{
	yygotominor.yy8 = phql_ret_join_item(yymsp[-3].minor.yy8, yymsp[-2].minor.yy8, yymsp[-1].minor.yy8, yymsp[0].minor.yy8);
}
/* #line 1920 "parser.c" */
        break;
      case 26:
/* #line 713 "parser.y" */
{
	yygotominor.yy8 = phql_ret_qualified_name(NULL, NULL, yymsp[0].minor.yy0);
  yy_destructor(38,&yymsp[-1].minor);
}
/* #line 1928 "parser.c" */
        break;
      case 27:
      case 48:
      case 162:
/* #line 717 "parser.y" */
{
	yygotominor.yy8 = phql_ret_qualified_name(NULL, NULL, yymsp[0].minor.yy0);
}
/* #line 1937 "parser.c" */
        break;
      case 29:
/* #line 727 "parser.y" */
{
	yygotominor.yy8 = phql_ret_join_type(PHQL_T_INNERJOIN);
  yy_destructor(39,&yymsp[-1].minor);
  yy_destructor(40,&yymsp[0].minor);
}
/* #line 1946 "parser.c" */
        break;
      case 30:
/* #line 731 "parser.y" */
{
	yygotominor.yy8 = phql_ret_join_type(PHQL_T_CROSSJOIN);
  yy_destructor(41,&yymsp[-1].minor);
  yy_destructor(40,&yymsp[0].minor);
}
/* #line 1955 "parser.c" */
        break;
      case 31:
/* #line 735 "parser.y" */
{
	yygotominor.yy8 = phql_ret_join_type(PHQL_T_LEFTJOIN);
  yy_destructor(42,&yymsp[-2].minor);
  yy_destructor(43,&yymsp[-1].minor);
  yy_destructor(40,&yymsp[0].minor);
}
/* #line 1965 "parser.c" */
        break;
      case 32:
/* #line 739 "parser.y" */
{
	yygotominor.yy8 = phql_ret_join_type(PHQL_T_LEFTJOIN);
  yy_destructor(42,&yymsp[-1].minor);
  yy_destructor(40,&yymsp[0].minor);
}
/* #line 1974 "parser.c" */
        break;
      case 33:
/* #line 743 "parser.y" */
{
	yygotominor.yy8 = phql_ret_join_type(PHQL_T_RIGHTJOIN);
  yy_destructor(44,&yymsp[-2].minor);
  yy_destructor(43,&yymsp[-1].minor);
  yy_destructor(40,&yymsp[0].minor);
}
/* #line 1984 "parser.c" */
        break;
      case 34:
/* #line 747 "parser.y" */
{
	yygotominor.yy8 = phql_ret_join_type(PHQL_T_RIGHTJOIN);
  yy_destructor(44,&yymsp[-1].minor);
  yy_destructor(40,&yymsp[0].minor);
}
/* #line 1993 "parser.c" */
        break;
      case 35:
/* #line 751 "parser.y" */
{
	yygotominor.yy8 = phql_ret_join_type(PHQL_T_FULLJOIN);
  yy_destructor(45,&yymsp[-2].minor);
  yy_destructor(43,&yymsp[-1].minor);
  yy_destructor(40,&yymsp[0].minor);
}
/* #line 2003 "parser.c" */
        break;
      case 36:
/* #line 755 "parser.y" */
{
	yygotominor.yy8 = phql_ret_join_type(PHQL_T_FULLJOIN);
  yy_destructor(45,&yymsp[-1].minor);
  yy_destructor(40,&yymsp[0].minor);
}
/* #line 2012 "parser.c" */
        break;
      case 37:
/* #line 759 "parser.y" */
{
	yygotominor.yy8 = phql_ret_join_type(PHQL_T_INNERJOIN);
  yy_destructor(40,&yymsp[0].minor);
}
/* #line 2020 "parser.c" */
        break;
      case 38:
/* #line 765 "parser.y" */
{
	yygotominor.yy8 = yymsp[0].minor.yy8;
  yy_destructor(46,&yymsp[-1].minor);
}
/* #line 2028 "parser.c" */
        break;
      case 40:
/* #line 776 "parser.y" */
{
	yygotominor.yy8 = phql_ret_insert_statement2(yymsp[-4].minor.yy8, NULL, yymsp[-1].minor.yy8);
  yy_destructor(31,&yymsp[-3].minor);
  yy_destructor(47,&yymsp[-2].minor);
  yy_destructor(48,&yymsp[0].minor);
}
/* #line 2038 "parser.c" */
        break;
      case 41:
/* #line 780 "parser.y" */
{
	yygotominor.yy8 = phql_ret_insert_statement(yymsp[-4].minor.yy8, NULL, yymsp[-1].minor.yy8);
  yy_destructor(49,&yymsp[-6].minor);
  yy_destructor(50,&yymsp[-5].minor);
  yy_destructor(51,&yymsp[-3].minor);
  yy_destructor(47,&yymsp[-2].minor);
  yy_destructor(48,&yymsp[0].minor);
}
/* #line 2050 "parser.c" */
        break;
      case 42:
/* #line 784 "parser.y" */
{
	yygotominor.yy8 = phql_ret_insert_statement(yymsp[-7].minor.yy8, yymsp[-5].minor.yy8, yymsp[-1].minor.yy8);
  yy_destructor(49,&yymsp[-9].minor);
  yy_destructor(50,&yymsp[-8].minor);
  yy_destructor(47,&yymsp[-6].minor);
  yy_destructor(48,&yymsp[-4].minor);
  yy_destructor(51,&yymsp[-3].minor);
  yy_destructor(47,&yymsp[-2].minor);
  yy_destructor(48,&yymsp[0].minor);
}
/* #line 2064 "parser.c" */
        break;
      case 49:
/* #line 822 "parser.y" */
{
	yygotominor.yy8 = phql_ret_update_statement(yymsp[-2].minor.yy8, yymsp[-1].minor.yy8, yymsp[0].minor.yy8);
}
/* #line 2071 "parser.c" */
        break;
      case 50:
/* #line 828 "parser.y" */
{
	yygotominor.yy8 = phql_ret_update_clause(yymsp[-2].minor.yy8, yymsp[0].minor.yy8);
  yy_destructor(52,&yymsp[-3].minor);
  yy_destructor(53,&yymsp[-1].minor);
}
/* #line 2080 "parser.c" */
        break;
      case 53:
/* #line 844 "parser.y" */
{
	yygotominor.yy8 = phql_ret_update_item(yymsp[-2].minor.yy8, yymsp[0].minor.yy8);
  yy_destructor(3,&yymsp[-1].minor);
}
/* #line 2088 "parser.c" */
        break;
      case 55:
/* #line 856 "parser.y" */
{
	yygotominor.yy8 = phql_ret_delete_statement(yymsp[-2].minor.yy8, yymsp[-1].minor.yy8, yymsp[0].minor.yy8);
}
/* #line 2095 "parser.c" */
        break;
      case 56:
/* #line 862 "parser.y" */
{
	yygotominor.yy8 = phql_ret_delete_clause(yymsp[0].minor.yy8);
  yy_destructor(54,&yymsp[-2].minor);
  yy_destructor(33,&yymsp[-1].minor);
}
/* #line 2104 "parser.c" */
        break;
      case 57:
/* #line 868 "parser.y" */
{
	yygotominor.yy8 = phql_ret_assoc_name(yymsp[-2].minor.yy8, yymsp[0].minor.yy0);
  yy_destructor(38,&yymsp[-1].minor);
}
/* #line 2112 "parser.c" */
        break;
      case 58:
/* #line 872 "parser.y" */
{
	yygotominor.yy8 = phql_ret_assoc_name(yymsp[-1].minor.yy8, yymsp[0].minor.yy0);
}
/* #line 2119 "parser.c" */
        break;
      case 59:
/* #line 876 "parser.y" */
{
	yygotominor.yy8 = phql_ret_assoc_name(yymsp[0].minor.yy8, NULL);
}
/* #line 2126 "parser.c" */
        break;
      case 61:
/* #line 888 "parser.y" */
{
	yygotominor.yy8 = yymsp[0].minor.yy8;
  yy_destructor(55,&yymsp[-1].minor);
}
/* #line 2134 "parser.c" */
        break;
      case 63:
/* #line 898 "parser.y" */
{
	yygotominor.yy8 = yymsp[0].minor.yy8;
  yy_destructor(56,&yymsp[-2].minor);
  yy_destructor(57,&yymsp[-1].minor);
}
/* #line 2143 "parser.c" */
        break;
      case 67:
/* #line 918 "parser.y" */
{
	yygotominor.yy8 = phql_ret_order_item(yymsp[0].minor.yy8, 0);
}
/* #line 2150 "parser.c" */
        break;
      case 68:
/* #line 922 "parser.y" */
{
	yygotominor.yy8 = phql_ret_order_item(yymsp[-1].minor.yy8, PHQL_T_ASC);
  yy_destructor(58,&yymsp[0].minor);
}
/* #line 2158 "parser.c" */
        break;
      case 69:
/* #line 926 "parser.y" */
{
	yygotominor.yy8 = phql_ret_order_item(yymsp[-1].minor.yy8, PHQL_T_DESC);
  yy_destructor(59,&yymsp[0].minor);
}
/* #line 2166 "parser.c" */
        break;
      case 70:
/* #line 932 "parser.y" */
{
	yygotominor.yy8 = yymsp[0].minor.yy8;
  yy_destructor(60,&yymsp[-2].minor);
  yy_destructor(57,&yymsp[-1].minor);
}
/* #line 2175 "parser.c" */
        break;
      case 75:
/* #line 958 "parser.y" */
{
	yygotominor.yy8 = yymsp[0].minor.yy8;
  yy_destructor(61,&yymsp[-1].minor);
}
/* #line 2183 "parser.c" */
        break;
      case 77:
      case 81:
/* #line 968 "parser.y" */
{
	yygotominor.yy8 = phql_ret_limit_clause(yymsp[0].minor.yy8, NULL);
  yy_destructor(62,&yymsp[-1].minor);
}
/* #line 2192 "parser.c" */
        break;
      case 78:
/* #line 972 "parser.y" */
{
	yygotominor.yy8 = phql_ret_limit_clause(yymsp[0].minor.yy8, yymsp[-2].minor.yy8);
  yy_destructor(62,&yymsp[-3].minor);
  yy_destructor(31,&yymsp[-1].minor);
}
/* #line 2201 "parser.c" */
        break;
      case 79:
/* #line 976 "parser.y" */
{
	yygotominor.yy8 = phql_ret_limit_clause(yymsp[-2].minor.yy8, yymsp[0].minor.yy8);
  yy_destructor(62,&yymsp[-3].minor);
  yy_destructor(63,&yymsp[-1].minor);
}
/* #line 2210 "parser.c" */
        break;
      case 83:
/* #line 996 "parser.y" */
{
	yygotominor.yy8 = phql_ret_forupdate_clause();
  yy_destructor(64,&yymsp[-1].minor);
  yy_destructor(52,&yymsp[0].minor);
}
/* #line 2219 "parser.c" */
        break;
      case 85:
      case 148:
/* #line 1004 "parser.y" */
{
	yygotominor.yy8 = phql_ret_literal_zval(PHQL_T_HINTEGER, yymsp[0].minor.yy0);
}
/* #line 2227 "parser.c" */
        break;
      case 86:
      case 149:
/* #line 1008 "parser.y" */
{
	yygotominor.yy8 = phql_ret_literal_zval(PHQL_T_INTEGER, yymsp[0].minor.yy0);
}
/* #line 2235 "parser.c" */
        break;
      case 87:
      case 155:
/* #line 1012 "parser.y" */
{
	yygotominor.yy8 = phql_ret_placeholder_zval(PHQL_T_NPLACEHOLDER, yymsp[0].minor.yy0);
}
/* #line 2243 "parser.c" */
        break;
      case 88:
      case 156:
/* #line 1016 "parser.y" */
{
	yygotominor.yy8 = phql_ret_placeholder_zval(PHQL_T_SPLACEHOLDER, yymsp[0].minor.yy0);
}
/* #line 2251 "parser.c" */
        break;
      case 89:
/* #line 1022 "parser.y" */
{
	yygotominor.yy8 = phql_ret_expr(PHQL_T_MINUS, NULL, yymsp[0].minor.yy8);
  yy_destructor(26,&yymsp[-1].minor);
}
/* #line 2259 "parser.c" */
        break;
      case 90:
/* #line 1026 "parser.y" */
{
	yygotominor.yy8 = phql_ret_expr(PHQL_T_SUB, yymsp[-2].minor.yy8, yymsp[0].minor.yy8);
  yy_destructor(26,&yymsp[-1].minor);
}
/* #line 2267 "parser.c" */
        break;
      case 91:
/* #line 1030 "parser.y" */
{
	yygotominor.yy8 = phql_ret_expr(PHQL_T_ADD, yymsp[-2].minor.yy8, yymsp[0].minor.yy8);
  yy_destructor(25,&yymsp[-1].minor);
}
/* #line 2275 "parser.c" */
        break;
      case 92:
/* #line 1034 "parser.y" */
{
	yygotominor.yy8 = phql_ret_expr(PHQL_T_MUL, yymsp[-2].minor.yy8, yymsp[0].minor.yy8);
  yy_destructor(23,&yymsp[-1].minor);
}
/* #line 2283 "parser.c" */
        break;
      case 93:
/* #line 1038 "parser.y" */
{
	yygotominor.yy8 = phql_ret_expr(PHQL_T_DIV, yymsp[-2].minor.yy8, yymsp[0].minor.yy8);
  yy_destructor(22,&yymsp[-1].minor);
}
/* #line 2291 "parser.c" */
        break;
      case 94:
/* #line 1042 "parser.y" */
{
	yygotominor.yy8 = phql_ret_expr(PHQL_T_MOD, yymsp[-2].minor.yy8, yymsp[0].minor.yy8);
  yy_destructor(24,&yymsp[-1].minor);
}
/* #line 2299 "parser.c" */
        break;
      case 95:
/* #line 1046 "parser.y" */
{
	yygotominor.yy8 = phql_ret_expr(PHQL_T_AND, yymsp[-2].minor.yy8, yymsp[0].minor.yy8);
  yy_destructor(15,&yymsp[-1].minor);
}
/* #line 2307 "parser.c" */
        break;
      case 96:
/* #line 1050 "parser.y" */
{
	yygotominor.yy8 = phql_ret_expr(PHQL_T_OR, yymsp[-2].minor.yy8, yymsp[0].minor.yy8);
  yy_destructor(16,&yymsp[-1].minor);
}
/* #line 2315 "parser.c" */
        break;
      case 97:
/* #line 1054 "parser.y" */
{
	yygotominor.yy8 = phql_ret_expr(PHQL_T_BITWISE_AND, yymsp[-2].minor.yy8, yymsp[0].minor.yy8);
  yy_destructor(19,&yymsp[-1].minor);
}
/* #line 2323 "parser.c" */
        break;
      case 98:
/* #line 1058 "parser.y" */
{
	yygotominor.yy8 = phql_ret_expr(PHQL_T_BITWISE_OR, yymsp[-2].minor.yy8, yymsp[0].minor.yy8);
  yy_destructor(20,&yymsp[-1].minor);
}
/* #line 2331 "parser.c" */
        break;
      case 99:
/* #line 1062 "parser.y" */
{
	yygotominor.yy8 = phql_ret_expr(PHQL_T_BITWISE_XOR, yymsp[-2].minor.yy8, yymsp[0].minor.yy8);
  yy_destructor(21,&yymsp[-1].minor);
}
/* #line 2339 "parser.c" */
        break;
      case 100:
/* #line 1066 "parser.y" */
{
	yygotominor.yy8 = phql_ret_expr(PHQL_T_EQUALS, yymsp[-2].minor.yy8, yymsp[0].minor.yy8);
  yy_destructor(3,&yymsp[-1].minor);
}
/* #line 2347 "parser.c" */
        break;
      case 101:
/* #line 1070 "parser.y" */
{
	yygotominor.yy8 = phql_ret_expr(PHQL_T_NOTEQUALS, yymsp[-2].minor.yy8, yymsp[0].minor.yy8);
  yy_destructor(4,&yymsp[-1].minor);
}
/* #line 2355 "parser.c" */
        break;
      case 102:
/* #line 1074 "parser.y" */
{
	yygotominor.yy8 = phql_ret_expr(PHQL_T_LESS, yymsp[-2].minor.yy8, yymsp[0].minor.yy8);
  yy_destructor(5,&yymsp[-1].minor);
}
/* #line 2363 "parser.c" */
        break;
      case 103:
/* #line 1078 "parser.y" */
{
	yygotominor.yy8 = phql_ret_expr(PHQL_T_GREATER, yymsp[-2].minor.yy8, yymsp[0].minor.yy8);
  yy_destructor(6,&yymsp[-1].minor);
}
/* #line 2371 "parser.c" */
        break;
      case 104:
/* #line 1082 "parser.y" */
{
	yygotominor.yy8 = phql_ret_expr(PHQL_T_GREATEREQUAL, yymsp[-2].minor.yy8, yymsp[0].minor.yy8);
  yy_destructor(7,&yymsp[-1].minor);
}
/* #line 2379 "parser.c" */
        break;
      case 105:
/* #line 1086 "parser.y" */
{
	yygotominor.yy8 = phql_ret_expr(PHQL_T_TS_MATCHES, yymsp[-2].minor.yy8, yymsp[0].minor.yy8);
  yy_destructor(9,&yymsp[-1].minor);
}
/* #line 2387 "parser.c" */
        break;
      case 106:
/* #line 1090 "parser.y" */
{
	yygotominor.yy8 = phql_ret_expr(PHQL_T_TS_OR, yymsp[-2].minor.yy8, yymsp[0].minor.yy8);
  yy_destructor(10,&yymsp[-1].minor);
}
/* #line 2395 "parser.c" */
        break;
      case 107:
/* #line 1094 "parser.y" */
{
	yygotominor.yy8 = phql_ret_expr(PHQL_T_TS_AND, yymsp[-2].minor.yy8, yymsp[0].minor.yy8);
  yy_destructor(11,&yymsp[-1].minor);
}
/* #line 2403 "parser.c" */
        break;
      case 108:
/* #line 1098 "parser.y" */
{
	yygotominor.yy8 = phql_ret_expr(PHQL_T_TS_NEGATE, yymsp[-2].minor.yy8, yymsp[0].minor.yy8);
  yy_destructor(12,&yymsp[-1].minor);
}
/* #line 2411 "parser.c" */
        break;
      case 109:
/* #line 1102 "parser.y" */
{
	yygotominor.yy8 = phql_ret_expr(PHQL_T_TS_CONTAINS_ANOTHER, yymsp[-2].minor.yy8, yymsp[0].minor.yy8);
  yy_destructor(13,&yymsp[-1].minor);
}
/* #line 2419 "parser.c" */
        break;
      case 110:
/* #line 1106 "parser.y" */
{
	yygotominor.yy8 = phql_ret_expr(PHQL_T_TS_CONTAINS_IN, yymsp[-2].minor.yy8, yymsp[0].minor.yy8);
  yy_destructor(14,&yymsp[-1].minor);
}
/* #line 2427 "parser.c" */
        break;
      case 111:
/* #line 1110 "parser.y" */
{
	yygotominor.yy8 = phql_ret_expr(PHQL_T_LESSEQUAL, yymsp[-2].minor.yy8, yymsp[0].minor.yy8);
  yy_destructor(8,&yymsp[-1].minor);
}
/* #line 2435 "parser.c" */
        break;
      case 112:
/* #line 1114 "parser.y" */
{
	yygotominor.yy8 = phql_ret_expr(PHQL_T_LIKE, yymsp[-2].minor.yy8, yymsp[0].minor.yy8);
  yy_destructor(17,&yymsp[-1].minor);
}
/* #line 2443 "parser.c" */
        break;
      case 113:
/* #line 1118 "parser.y" */
{
	yygotominor.yy8 = phql_ret_expr(PHQL_T_NLIKE, yymsp[-3].minor.yy8, yymsp[0].minor.yy8);
  yy_destructor(29,&yymsp[-2].minor);
  yy_destructor(17,&yymsp[-1].minor);
}
/* #line 2452 "parser.c" */
        break;
      case 114:
/* #line 1122 "parser.y" */
{
	yygotominor.yy8 = phql_ret_expr(PHQL_T_ILIKE, yymsp[-2].minor.yy8, yymsp[0].minor.yy8);
  yy_destructor(18,&yymsp[-1].minor);
}
/* #line 2460 "parser.c" */
        break;
      case 115:
/* #line 1126 "parser.y" */
{
	yygotominor.yy8 = phql_ret_expr(PHQL_T_NILIKE, yymsp[-3].minor.yy8, yymsp[0].minor.yy8);
  yy_destructor(29,&yymsp[-2].minor);
  yy_destructor(18,&yymsp[-1].minor);
}
/* #line 2469 "parser.c" */
        break;
      case 116:
      case 119:
/* #line 1130 "parser.y" */
{
	yygotominor.yy8 = phql_ret_expr(PHQL_T_IN, yymsp[-4].minor.yy8, yymsp[-1].minor.yy8);
  yy_destructor(28,&yymsp[-3].minor);
  yy_destructor(47,&yymsp[-2].minor);
  yy_destructor(48,&yymsp[0].minor);
}
/* #line 2480 "parser.c" */
        break;
      case 117:
      case 120:
/* #line 1134 "parser.y" */
{
	yygotominor.yy8 = phql_ret_expr(PHQL_T_NOTIN, yymsp[-5].minor.yy8, yymsp[-1].minor.yy8);
  yy_destructor(29,&yymsp[-4].minor);
  yy_destructor(28,&yymsp[-3].minor);
  yy_destructor(47,&yymsp[-2].minor);
  yy_destructor(48,&yymsp[0].minor);
}
/* #line 2492 "parser.c" */
        break;
      case 118:
/* #line 1138 "parser.y" */
{
	yygotominor.yy8 = phql_ret_expr(PHQL_T_SUBQUERY, yymsp[-1].minor.yy8, NULL);
  yy_destructor(47,&yymsp[-2].minor);
  yy_destructor(48,&yymsp[0].minor);
}
/* #line 2501 "parser.c" */
        break;
      case 121:
/* #line 1150 "parser.y" */
{
	yygotominor.yy8 = phql_ret_expr(PHQL_T_EXISTS, NULL, yymsp[-1].minor.yy8);
  yy_destructor(69,&yymsp[-3].minor);
  yy_destructor(47,&yymsp[-2].minor);
  yy_destructor(48,&yymsp[0].minor);
}
/* #line 2511 "parser.c" */
        break;
      case 122:
/* #line 1154 "parser.y" */
{
	yygotominor.yy8 = phql_ret_expr(PHQL_T_AGAINST, yymsp[-2].minor.yy8, yymsp[0].minor.yy8);
  yy_destructor(1,&yymsp[-1].minor);
}
/* #line 2519 "parser.c" */
        break;
      case 123:
/* #line 1158 "parser.y" */
{
	yygotominor.yy8 = phql_ret_expr(PHQL_T_CAST, yymsp[-3].minor.yy8, phql_ret_raw_qualified_name(yymsp[-1].minor.yy0, NULL));
  yy_destructor(70,&yymsp[-5].minor);
  yy_destructor(47,&yymsp[-4].minor);
  yy_destructor(38,&yymsp[-2].minor);
  yy_destructor(48,&yymsp[0].minor);
}
/* #line 2530 "parser.c" */
        break;
      case 124:
/* #line 1162 "parser.y" */
{
	yygotominor.yy8 = phql_ret_expr(PHQL_T_CONVERT, yymsp[-3].minor.yy8, phql_ret_raw_qualified_name(yymsp[-1].minor.yy0, NULL));
  yy_destructor(71,&yymsp[-5].minor);
  yy_destructor(47,&yymsp[-4].minor);
  yy_destructor(72,&yymsp[-2].minor);
  yy_destructor(48,&yymsp[0].minor);
}
/* #line 2541 "parser.c" */
        break;
      case 125:
/* #line 1166 "parser.y" */
{
	yygotominor.yy8 = phql_ret_expr(PHQL_T_CASE, yymsp[-2].minor.yy8, yymsp[-1].minor.yy8);
  yy_destructor(73,&yymsp[-3].minor);
  yy_destructor(74,&yymsp[0].minor);
}
/* #line 2550 "parser.c" */
        break;
      case 128:
/* #line 1178 "parser.y" */
{
	yygotominor.yy8 = phql_ret_expr(PHQL_T_WHEN, yymsp[-2].minor.yy8, yymsp[0].minor.yy8);
  yy_destructor(75,&yymsp[-3].minor);
  yy_destructor(76,&yymsp[-1].minor);
}
/* #line 2559 "parser.c" */
        break;
      case 129:
/* #line 1182 "parser.y" */
{
	yygotominor.yy8 = phql_ret_expr(PHQL_T_ELSE, yymsp[0].minor.yy8, NULL);
  yy_destructor(77,&yymsp[-1].minor);
}
/* #line 2567 "parser.c" */
        break;
      case 130:
      case 132:
/* #line 1188 "parser.y" */
{
	yygotominor.yy8 = phql_ret_func_call(yymsp[-4].minor.yy0, yymsp[-1].minor.yy8, yymsp[-2].minor.yy8);
  yy_destructor(47,&yymsp[-3].minor);
  yy_destructor(48,&yymsp[0].minor);
}
/* #line 2577 "parser.c" */
        break;
      case 133:
/* #line 1204 "parser.y" */
{
	yygotominor.yy8 = phql_ret_distinct();
  yy_destructor(34,&yymsp[0].minor);
}
/* #line 2585 "parser.c" */
        break;
      case 141:
/* #line 1242 "parser.y" */
{
	yygotominor.yy8 = phql_ret_expr(PHQL_T_ISNULL, yymsp[-2].minor.yy8, NULL);
  yy_destructor(27,&yymsp[-1].minor);
  yy_destructor(79,&yymsp[0].minor);
}
/* #line 2594 "parser.c" */
        break;
      case 142:
/* #line 1246 "parser.y" */
{
	yygotominor.yy8 = phql_ret_expr(PHQL_T_ISNOTNULL, yymsp[-3].minor.yy8, NULL);
  yy_destructor(27,&yymsp[-2].minor);
  yy_destructor(29,&yymsp[-1].minor);
  yy_destructor(79,&yymsp[0].minor);
}
/* #line 2604 "parser.c" */
        break;
      case 143:
/* #line 1250 "parser.y" */
{
	yygotominor.yy8 = phql_ret_expr(PHQL_T_BETWEEN, yymsp[-2].minor.yy8, yymsp[0].minor.yy8);
  yy_destructor(2,&yymsp[-1].minor);
}
/* #line 2612 "parser.c" */
        break;
      case 144:
/* #line 1254 "parser.y" */
{
	yygotominor.yy8 = phql_ret_expr(PHQL_T_NOT, NULL, yymsp[0].minor.yy8);
  yy_destructor(29,&yymsp[-1].minor);
}
/* #line 2620 "parser.c" */
        break;
      case 145:
/* #line 1258 "parser.y" */
{
	yygotominor.yy8 = phql_ret_expr(PHQL_T_BITWISE_NOT, NULL, yymsp[0].minor.yy8);
  yy_destructor(30,&yymsp[-1].minor);
}
/* #line 2628 "parser.c" */
        break;
      case 146:
/* #line 1262 "parser.y" */
{
	yygotominor.yy8 = phql_ret_expr(PHQL_T_ENCLOSED, yymsp[-1].minor.yy8, NULL);
  yy_destructor(47,&yymsp[-2].minor);
  yy_destructor(48,&yymsp[0].minor);
}
/* #line 2637 "parser.c" */
        break;
      case 150:
/* #line 1278 "parser.y" */
{
	yygotominor.yy8 = phql_ret_literal_zval(PHQL_T_STRING, yymsp[0].minor.yy0);
}
/* #line 2644 "parser.c" */
        break;
      case 151:
/* #line 1282 "parser.y" */
{
	yygotominor.yy8 = phql_ret_literal_zval(PHQL_T_DOUBLE, yymsp[0].minor.yy0);
}
/* #line 2651 "parser.c" */
        break;
      case 152:
/* #line 1286 "parser.y" */
{
	yygotominor.yy8 = phql_ret_literal_zval(PHQL_T_NULL, NULL);
  yy_destructor(79,&yymsp[0].minor);
}
/* #line 2659 "parser.c" */
        break;
      case 153:
/* #line 1290 "parser.y" */
{
	yygotominor.yy8 = phql_ret_literal_zval(PHQL_T_TRUE, NULL);
  yy_destructor(82,&yymsp[0].minor);
}
/* #line 2667 "parser.c" */
        break;
      case 154:
/* #line 1294 "parser.y" */
{
	yygotominor.yy8 = phql_ret_literal_zval(PHQL_T_FALSE, NULL);
  yy_destructor(83,&yymsp[0].minor);
}
/* #line 2675 "parser.c" */
        break;
      case 157:
/* #line 1306 "parser.y" */
{
	yygotominor.yy8 = phql_ret_placeholder_zval(PHQL_T_NTPLACEHOLDER, yymsp[0].minor.yy0);
}
/* #line 2682 "parser.c" */
        break;
      case 158:
/* #line 1310 "parser.y" */
{
	yygotominor.yy8 = phql_ret_placeholder_zval(PHQL_T_STPLACEHOLDER, yymsp[0].minor.yy0);
}
/* #line 2689 "parser.c" */
        break;
      case 159:
/* #line 1316 "parser.y" */
{
	yygotominor.yy8 = phql_ret_qualified_name(yymsp[-4].minor.yy0, yymsp[-2].minor.yy0, yymsp[0].minor.yy0);
  yy_destructor(86,&yymsp[-3].minor);
  yy_destructor(37,&yymsp[-1].minor);
}
/* #line 2698 "parser.c" */
        break;
      case 160:
/* #line 1320 "parser.y" */
{
	yygotominor.yy8 = phql_ret_qualified_name(yymsp[-2].minor.yy0, NULL, yymsp[0].minor.yy0);
  yy_destructor(86,&yymsp[-1].minor);
}
/* #line 2706 "parser.c" */
        break;
      case 161:
/* #line 1324 "parser.y" */
{
	yygotominor.yy8 = phql_ret_qualified_name(NULL, yymsp[-2].minor.yy0, yymsp[0].minor.yy0);
  yy_destructor(37,&yymsp[-1].minor);
}
/* #line 2714 "parser.c" */
        break;
  };
  yygoto = yyRuleInfo[yyruleno].lhs;
  yysize = yyRuleInfo[yyruleno].nrhs;
  yypParser->yyidx -= yysize;
  yyact = yy_find_reduce_action(yypParser,yygoto);
  if( yyact < YYNSTATE ){
    yy_shift(yypParser,yyact,yygoto,&yygotominor);
  }else if( yyact == YYNSTATE + YYNRULE + 1 ){
    yy_accept(yypParser);
  }
}

/*
** The following code executes when the parse fails
*/
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
  phql_ARG_STORE; /* Suppress warning about unused %extra_argument variable */
}

/*
** The following code executes when a syntax error first occurs.
*/
static void yy_syntax_error(
  yyParser *yypParser,           /* The parser */
  int yymajor,                   /* The major type of the error token */
  YYMINORTYPE yyminor            /* The minor type of the error token */
){
  phql_ARG_FETCH;
#define TOKEN (yyminor.yy0)
/* #line 510 "parser.y" */

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

/* #line 2823 "parser.c" */
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
  int yyact;            /* The parser action. */
  int yyendofinput;     /* True if we are at the end of input */
  int yyerrorhit = 0;   /* True if yymajor has invoked an error */
  yyParser *yypParser;  /* The parser */

  /* (re)initialize the parser, if necessary */
  yypParser = (yyParser*)yyp;
  if( yypParser->yyidx<0 ){
    if( yymajor==0 ) return;
    yypParser->yyidx = 0;
    yypParser->yyerrcnt = -1;
    yypParser->yystack[0].stateno = 0;
    yypParser->yystack[0].major = 0;
  }
  yyminorunion.yy0 = yyminor;
  yyendofinput = (yymajor==0);
  phql_ARG_STORE;

#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sInput %s\n",yyTracePrompt,yyTokenName[yymajor]);
  }
#endif

  do{
    yyact = yy_find_shift_action(yypParser,yymajor);
    if( yyact<YYNSTATE ){
      yy_shift(yypParser,yyact,yymajor,&yyminorunion);
      yypParser->yyerrcnt--;
      if( yyendofinput && yypParser->yyidx>=0 ){
        yymajor = 0;
      }else{
        yymajor = YYNOCODE;
      }
    }else if( yyact < YYNSTATE + YYNRULE ){
      yy_reduce(yypParser,yyact-YYNSTATE);
    }else if( yyact == YY_ERROR_ACTION ){
      int yymx;
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
        yy_syntax_error(yypParser,yymajor,yyminorunion);
      }
      yymx = yypParser->yystack[yypParser->yyidx].major;
      if( yymx==YYERRORSYMBOL || yyerrorhit ){
#ifndef NDEBUG
        if( yyTraceFILE ){
          fprintf(yyTraceFILE,"%sDiscard input token %s\n",
             yyTracePrompt,yyTokenName[yymajor]);
        }
#endif
        yy_destructor(yymajor,&yyminorunion);
        yymajor = YYNOCODE;
      }else{
         while(
          yypParser->yyidx >= 0 &&
          yymx != YYERRORSYMBOL &&
          (yyact = yy_find_shift_action(yypParser,YYERRORSYMBOL)) >= YYNSTATE
        ){
          yy_pop_parser_stack(yypParser);
        }
        if( yypParser->yyidx < 0 || yymajor==0 ){
          yy_destructor(yymajor,&yyminorunion);
          yy_parse_failed(yypParser);
          yymajor = YYNOCODE;
        }else if( yymx!=YYERRORSYMBOL ){
          YYMINORTYPE u2;
          u2.YYERRSYMDT = 0;
          yy_shift(yypParser,yyact,YYERRORSYMBOL,&u2);
        }
      }
      yypParser->yyerrcnt = 3;
      yyerrorhit = 1;
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
        yy_syntax_error(yypParser,yymajor,yyminorunion);
      }
      yypParser->yyerrcnt = 3;
      yy_destructor(yymajor,&yyminorunion);
      if( yyendofinput ){
        yy_parse_failed(yypParser);
      }
      yymajor = YYNOCODE;
#endif
    }else{
      yy_accept(yypParser);
      yymajor = YYNOCODE;
    }
  }while( yymajor!=YYNOCODE && yypParser->yyidx>=0 );
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
