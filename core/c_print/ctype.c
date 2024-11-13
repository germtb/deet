#include "c_print.h"
#include "ctype.h"
#include "../parser/parser.h"
#include "../hashmap/hashmap.h"

#include <stdbool.h>

CType make_primitive_ctype(char *name)
{
    return (CType){.name = name, .is_primitive = true};
}

CType make_pointer_ctype(char *name)
{
    return (CType){.name = name, .is_primitive = false};
}

CType make_ctype_not_found()
{
    return make_pointer_ctype("NOT_FOUND");
}

void print_type(char *key, void *value)
{
    if (key == NULL || value == NULL)
    {
        return;
    }
    CType *type = (CType *)value;
    printf("{ key: %s, value: %s}\n", key, type->name);
}

CType infer_ctype(Context *context, Node *node)
{
    switch (node->type)
    {
    case NFunctionDeclaration:
        return make_primitive_ctype(NULL);
    case NNumber:
        return make_primitive_ctype("int32_t");
    case NDecimal:
        return make_primitive_ctype("float64_t");
    case NInt8:
        return make_primitive_ctype("int8_t");
    case NInt16:
        return make_primitive_ctype("int16_t");
    case NInt32:
        return make_primitive_ctype("int32_t");
    case NInt64:
        return make_primitive_ctype("int64_t");
    case NUint8:
        return make_primitive_ctype("uint8_t");
    case NUint16:
        return make_primitive_ctype("uint16_t");
    case NUint32:
        return make_primitive_ctype("uint32_t");
    case NUint64:
        return make_primitive_ctype("uint64_t");
    case NFloat32:
        return make_primitive_ctype("float32_t");
    case NFloat64:
        return make_primitive_ctype("float64_t");
    case NStringLiteral:
        return make_primitive_ctype("String");
    case NTrue:
        return make_primitive_ctype("bool");
    case NFalse:
        return make_primitive_ctype("bool");
    case NStructProperty:
        return make_primitive_ctype("TODO: StructProperty");
    case NAccess:
        return make_primitive_ctype("TODO: Access");
    case NArray:
        return make_primitive_ctype("TODO: Array");
    case NIf:
        return make_primitive_ctype("TODO: If");
    case NElse:
        return make_primitive_ctype("TODO: Else");
    case NElseIf:
        return make_primitive_ctype("TODO: ElseIf");
    case NMatch:
        return make_primitive_ctype("TODO: Match");
    case NMatchCase:
        return make_primitive_ctype("TODO: MatchCase");
    case NDefaultCase:
        return make_primitive_ctype("TODO: DefaultCase");
    case NPattern:
        return make_primitive_ctype("TODO: Pattern");
    case NReturn:
        return make_primitive_ctype("TODO: Return");
    case NIdentifier:
        return make_primitive_ctype("TODO: Identifier");
    case NStructType:
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

        // print_node_ast(node);

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
        return make_primitive_ctype("TODO: EffectIdentifier");
    case NFunction:
        return make_primitive_ctype("TODO: Function");
    case NFunctionArgs:
        return make_primitive_ctype("TODO: FunctionArgs");
    case NFunctionArg:
        return make_primitive_ctype("TODO: FunctionArg");
    case NCall:
        return make_primitive_ctype("TODO: Call");
    case NCallArgs:
        return make_primitive_ctype("TODO: CallArgs");
    case NCallArg:
        return make_primitive_ctype("TODO: CallArg");
    case NAdd:
        return make_primitive_ctype("TODO: Add");
    case NSub:
        return make_primitive_ctype("TODO: Sub");
    case NMul:
        return make_primitive_ctype("TODO: Mul");
    case NDiv:
        return make_primitive_ctype("TODO: Div");
    case NGe:
        return make_primitive_ctype("TODO: Ge");
    case NGt:
        return make_primitive_ctype("TODO: Gt");
    case NSe:
        return make_primitive_ctype("TODO: Se");
    case NSt:
        return make_primitive_ctype("TODO: St");
    case NEq:
        return make_primitive_ctype("TODO: Eq");
    case NNeq:
        return make_primitive_ctype("TODO: Neq");
    case NAnd:
        return make_primitive_ctype("TODO: And");
    case NOr:
        return make_primitive_ctype("TODO: Or");
    case NXor:
        return make_primitive_ctype("TODO: Xor");
    case NBitAnd:
        return make_primitive_ctype("TODO: BitAnd");
    case NBitOr:
        return make_primitive_ctype("TODO: BitOr");
    case NNot:
        return make_primitive_ctype("TODO: Not");
    case NNegative:
        return make_primitive_ctype("TODO: Negative");
    case NPostIncrement:
        return make_primitive_ctype("TODO: PostIncrement");
    case NPostDecrement:
        return make_primitive_ctype("TODO: PostDecrement");
    case NPreIncrement:
        return make_primitive_ctype("TODO: PreIncrement");
    case NPreDecrement:
        return make_primitive_ctype("TODO: PreDecrement");
    case NReassignment:
        return make_primitive_ctype("TODO: Reassignment");
    case NConstDeclaration:
        return make_primitive_ctype("TODO: ConstDeclaration");
    case NVarDeclaration:
        return make_primitive_ctype("TODO: VarDeclaration");
    case NTypeDeclaration:
        return make_primitive_ctype("TODO: TypeDeclaration");
    case NStatement:
        return make_primitive_ctype("TODO: Statement");
    case NComment:
        return make_primitive_ctype("TODO: Comment");
    case NProgram:
        return make_primitive_ctype("TODO: Program");
    case NEnd:
        return make_primitive_ctype("TODO: End");
    case NStructDeclaration:
        return make_primitive_ctype("TODO: StructDeclaration");
    case NStringInterpolation:
        return make_pointer_ctype("String");
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
    case NEffect:
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

    return make_ctype_not_found();
}
