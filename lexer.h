#ifndef LEXER_H_
#define LEXER_H_

enum TokenType
{
    TInvalid,
    TStart,
    TFn,
    TNumber,
    TUint8,
    TUint16,
    TUint32,
    TUint64,
    TDecimal,
    TInt8,
    TInt16,
    TInt32,
    TInt64,
    TFloat32,
    TFloat64,
    TWith,
    TNamespace,
    TConst,
    TVar,
    TType,
    TNamespaceIdentifier,
    TIdentifier,
    TEffectIdentifier,
    TTrue,
    TFalse,
    TAdd,
    TSub,
    TMul,
    TEq,
    TNeq,
    TGt,
    TSt,
    TGe,
    TSe,
    TAnd,
    TOr,
    TXor,
    TBitAnd,
    TBitOr,
    TAssign,
    TDiv,
    TOpenCurlyBracket,
    TCloseCurlyBracket,
    TOpenSquareBracket,
    TCloseSquareBracket,
    TOpenParens,
    TCloseParens,
    TIncrement,
    TDecrement,
    TNot,
    TIf,
    TElse,
    TEnd,
    TComment,
    TStruct,
    TUnion,
    TAccess,
    TMatch,
    TPlaceholder,
    TRest,
    TFnArrow,
    TColon,
    TString,
    TReturn,
    TResume,
    TPipe,
    TStringInterpolation,
    TSemicolon,
    TContinue,
    TBreak,
    TWhile,
    TFor,
    TEffect,
    TComma,
};

typedef struct Token
{
    enum TokenType type;
    char *content;
    struct Token *next;
    // int start_line;
    // int start_col;
    // int end_line;
    // int end_col;
} Token;

Token *tokenize(char *src);

char *get_token_name(enum TokenType type);

void print_tokens(char *src);

#endif // LEXER_H_
