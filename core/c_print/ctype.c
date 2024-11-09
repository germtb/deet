#include "c_print.h"
#include "../parser/parser.h"
#include "../hashmap/hashmap.h"

#include <stdbool.h>

typedef struct CType
{
    char *name;
    char *primitive_name;
    int bytesize;
    bool is_primitive;
} CType;

CType make_primitive_ctype(char *name, int bytesize)
{
    return (CType){.name = name, .bytesize = bytesize, .is_primitive = true};
}

CType make_pointer_ctype(char *name, char *primitive_name)
{
    return (CType){.name = name, .primitive_name = primitive_name, .bytesize = 1, .is_primitive = false};
}

CType make_ctype_not_found()
{
    return make_pointer_ctype("NOT_FOUND *", "NOT_FOUND");
}

CType infer_ctype(Context *context, Node *node)
{
    switch (node->type)
    {
    case NFunctionDeclaration:
        return make_primitive_ctype(NULL, 1);
    case NNumber:
        return make_primitive_ctype("int32_t", 4);
    case NDecimal:
        return make_primitive_ctype("float64_t", 8);
    case NInt8:
        return make_primitive_ctype("int8_t", 1);
    case NInt16:
        return make_primitive_ctype("int16_t", 2);
    case NInt32:
        return make_primitive_ctype("int32_t", 4);
    case NInt64:
        return make_primitive_ctype("int64_t", 8);
    case NUint8:
        return make_primitive_ctype("uint8_t", 1);
    case NUint16:
        return make_primitive_ctype("uint16_t", 2);
    case NUint32:
        return make_primitive_ctype("uint32_t", 4);
    case NUint64:
        return make_primitive_ctype("uint64_t", 8);
    case NFloat32:
        return make_primitive_ctype("float32_t", 4);
    case NFloat64:
        return make_primitive_ctype("float64_t", 8);
    case NStringLiteral:
        return make_primitive_ctype("char*", 8);
    case NTrue:
        return make_primitive_ctype("bool", 1);
    case NFalse:
        return make_primitive_ctype("bool", 1);
    case NStructProperty:
        return make_primitive_ctype("TODO: StructProperty", 1);
    case NAccess:
        return make_primitive_ctype("TODO: Access", 1);
    case NArray:
        return make_primitive_ctype("TODO: Array", 1);
    case NIf:
        return make_primitive_ctype("TODO: If", 1);
    case NElse:
        return make_primitive_ctype("TODO: Else", 1);
    case NElseIf:
        return make_primitive_ctype("TODO: ElseIf", 1);
    case NMatch:
        return make_primitive_ctype("TODO: Match", 1);
    case NMatchCase:
        return make_primitive_ctype("TODO: MatchCase", 1);
    case NDefaultCase:
        return make_primitive_ctype("TODO: DefaultCase", 1);
    case NPattern:
        return make_primitive_ctype("TODO: Pattern", 1);
    case NReturn:
        return make_primitive_ctype("TODO: Return", 1);
    case NIdentifier:
        return make_primitive_ctype("TODO: Identifier", 1);
    case NStruct:
    {
        // if (node == NULL)
        // {
        //     printf("YES\n");
        // }
        // else
        // {
        //     printf("NO\n");
        // }

        // print_node_ast(node, 0);

        // printf("Node type: %s", get_node_name(node->type));
        // printf("Node type: %s", get_node_name(node->type));

        // CType *type = hashmap_get(&context->types, node->str_value);

        // if (type == NULL)
        // {
        //     return (CType){.name = "Type not found", .bytesize = 1};
        // }

        // return *type;

        return make_ctype_not_found();
    }
    case NTypeIdentifier:
    {
        CType *type = hashmap_get(context->types, node->str_value);

        if (type == NULL)
        {
            return make_ctype_not_found();
        }

        return *type;
    }
    case NEffectIdentifier:
        return make_primitive_ctype("TODO: EffectIdentifier", 1);
    case NFunction:
        return make_primitive_ctype("TODO: Function", 1);
    case NFunctionArgs:
        return make_primitive_ctype("TODO: FunctionArgs", 1);
    case NFunctionArg:
        return make_primitive_ctype("TODO: FunctionArg", 1);
    case NCall:
        return make_primitive_ctype("TODO: Call", 1);
    case NCallArgs:
        return make_primitive_ctype("TODO: CallArgs", 1);
    case NCallArg:
        return make_primitive_ctype("TODO: CallArg", 1);
    case NAdd:
        return make_primitive_ctype("TODO: Add", 1);
    case NSub:
        return make_primitive_ctype("TODO: Sub", 1);
    case NMul:
        return make_primitive_ctype("TODO: Mul", 1);
    case NDiv:
        return make_primitive_ctype("TODO: Div", 1);
    case NGe:
        return make_primitive_ctype("TODO: Ge", 1);
    case NGt:
        return make_primitive_ctype("TODO: Gt", 1);
    case NSe:
        return make_primitive_ctype("TODO: Se", 1);
    case NSt:
        return make_primitive_ctype("TODO: St", 1);
    case NEq:
        return make_primitive_ctype("TODO: Eq", 1);
    case NNeq:
        return make_primitive_ctype("TODO: Neq", 1);
    case NAnd:
        return make_primitive_ctype("TODO: And", 1);
    case NOr:
        return make_primitive_ctype("TODO: Or", 1);
    case NXor:
        return make_primitive_ctype("TODO: Xor", 1);
    case NBitAnd:
        return make_primitive_ctype("TODO: BitAnd", 1);
    case NBitOr:
        return make_primitive_ctype("TODO: BitOr", 1);
    case NNot:
        return make_primitive_ctype("TODO: Not", 1);
    case NNegative:
        return make_primitive_ctype("TODO: Negative", 1);
    case NPostIncrement:
        return make_primitive_ctype("TODO: PostIncrement", 1);
    case NPostDecrement:
        return make_primitive_ctype("TODO: PostDecrement", 1);
    case NPreIncrement:
        return make_primitive_ctype("TODO: PreIncrement", 1);
    case NPreDecrement:
        return make_primitive_ctype("TODO: PreDecrement", 1);
    case NReassignment:
        return make_primitive_ctype("TODO: Reassignment", 1);
    case NConstDeclaration:
        return make_primitive_ctype("TODO: ConstDeclaration", 1);
    case NVarDeclaration:
        return make_primitive_ctype("TODO: VarDeclaration", 1);
    case NTypeDeclaration:
        return make_primitive_ctype("TODO: TypeDeclaration", 1);
    case NStatement:
        return make_primitive_ctype("TODO: Statement", 1);
    case NComment:
        return make_primitive_ctype("TODO: Comment", 1);
    case NProgram:
        return make_primitive_ctype("TODO: Program", 1);
    case NEnd:
        return make_primitive_ctype("TODO: End", 1);
    case NStructDeclaration:
        return make_primitive_ctype("TODO: StructDeclaration", 1);
    case NStringInterpolation:
        return make_pointer_ctype("char*", "char*");
    case NBlock:
    {
        // TODO
        break;
    }
    case NPipe:
    {
        // TODO
        break;
    }
    case NTypeParameters:
    {
        // TODO
        break;
    }
    case NFor:
    {
        // TODO
        break;
    }
    case NWhile:
    {
        // TODO
        break;
    }
    case NContinue:
    {
        // TODO
        break;
    }
    case NBreak:
    {
        // TODO
        break;
    }
    }

    return make_pointer_ctype("TYPE NOT FOUND *", "TYPE NOT FOUND");
}
