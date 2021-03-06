#include "cparse.h"

#include "snowcrash.h"
#include "sosJSON.h"

#include "SerializeAST.h"
#include "SerializeSourcemap.h"
#include "SerializeResult.h"

#include "ConversionContext.h"

#include <string.h>

namespace sc = snowcrash;

static char* ToString(const std::stringstream& stream)
{
    size_t length = stream.str().length() + 1;
    char* str = (char*)malloc(length);
    memcpy(str, stream.str().c_str(), length);
    return str;
}

SC_API int c_parse(const char* source,
                   sc_blueprint_parser_options options,
                   enum drafter_ast_type_option astType,
                   char** result)
{

    std::stringstream inputStream;

    inputStream << source;

    sc::ParseResult<sc::Blueprint> blueprint;
    sc::parse(inputStream.str(), options | sc::ExportSourcemapOption, blueprint);

    sos::SerializeJSON serializer;

    if (result) {
        std::stringstream resultStream;
        drafter::WrapperOptions wrapperOptions(drafter::ASTType(astType), options & SC_EXPORT_SOURCEMAP_OPTION);

        try {
            serializer.process(drafter::WrapResult(blueprint, wrapperOptions), resultStream);
        }
        catch (snowcrash::Error& e) {
            blueprint.report.error = e;
        }
        catch (std::exception& e) {
            blueprint.report.error = snowcrash::Error(e.what(), snowcrash::ApplicationError);
        }

        resultStream << "\n";
        *result = ToString(resultStream);
    }

    return blueprint.report.error.code;
}
