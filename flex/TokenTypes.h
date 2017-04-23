#ifndef ___TOKEN_TYPES___
#define ___TOKEN_TYPES___
#include <string>
//#include <stdexcept>

//=================================================================================================================//

enum TokenID
{
	SYM_LBRACKET,
	SYM_RBRACKET,
	SYM_T,
	SYM_X,
	SYM_X_END,
	SYM_U,
	SYM_Z,

	FUNC_DELTA,
	FUNC_C,
	FUNC_SQRBRAC,

	OP_POW,
	OP_EQUAL,

	EXPR_PART,

	DELIM_MUL,
   DELIM_ELEMENT_MUL,
	DELIM_COMMA,
   
   SYM_EOF,
   
   // symbols used for identities
   IDENT_VAR,
   SYM_IDENTITY,
   FUNC_SGN,
};

//=================================================================================================================//

const static char * TokenNames[] = {
	"SYM_LBRACKET",
	"SYM_RBRACKET",
	"SYM_T",
	"SYM_X",
	"SYM_X_END",
	"SYM_U",
	"SYM_Z",

	"FUNC_DELTA",
	"FUNC_C",
	"FUNC_SQRBRAC",

	"OP_POW",
	"OP_EQUAL",

	"EXPR_PART",

	"DELIM_MUL",
   "DELIM_ELEMENT_MUL",
	"DELIM_COMMA",
   
   "SYM_EOF",
   
   "IDENT_VAR",
   "SYM_IDENTITY",
   "FUNC_SGN",
};

//=================================================================================================================//

class Token{
public:
	Token( TokenID tid, const std::string & data = "" );
	~Token();
	
	std::string print() const;
	TokenID getID() const;
   const std::string & getData() const;

private:
	TokenID mId;
	std::string mData;
};

//=================================================================================================================//

extern Token * gCurrentToken;

//=================================================================================================================//

#endif //___TOKEN_TYPES___
