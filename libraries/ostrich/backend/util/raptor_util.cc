/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements. See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership. The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "raptor_util.h"
#include <glog/logging.h>

namespace marmotta {
namespace util {
namespace raptor {

// Helper macros. Some Rasqal functions copy the input string themselves, others don't.
#define STR(s) (const unsigned char*)s.c_str()
#define CPSTR(s) (const unsigned char*)strdup(s.c_str())

rdf::Resource ConvertResource(raptor_term *node) {
    if (node == nullptr) {
        return rdf::Resource();
    }

    switch (node->type) {
        case RAPTOR_TERM_TYPE_URI:
            return rdf::URI(std::string((const char*)raptor_uri_as_string(node->value.uri)));
        case RAPTOR_TERM_TYPE_BLANK:
            return rdf::BNode(std::string((const char*)node->value.blank.string,
                                          node->value.blank.string_len));
        default:
            LOG(INFO) << "Error: unsupported resource type " << node->type;
            return rdf::Resource();
    }
}


rdf::Value ConvertValue(raptor_term *node) {
    if (node == nullptr) {
        return rdf::Value();
    }

    switch (node->type) {
        case RAPTOR_TERM_TYPE_URI:
            return rdf::URI((const char*)raptor_uri_as_string(node->value.uri));
        case RAPTOR_TERM_TYPE_BLANK:
            return rdf::BNode(std::string((const char*)node->value.blank.string,
                                          node->value.blank.string_len));
        case RAPTOR_TERM_TYPE_LITERAL:
            if(node->value.literal.language != nullptr) {
                return rdf::StringLiteral(
                        std::string((const char*)node->value.literal.string, node->value.literal.string_len),
                        std::string((const char*)node->value.literal.language, node->value.literal.language_len)
                );
            } else if(node->value.literal.datatype != nullptr) {
                return rdf::DatatypeLiteral(
                        std::string((const char*)node->value.literal.string, node->value.literal.string_len),
                        rdf::URI((const char*)raptor_uri_as_string(node->value.literal.datatype))
                );
            } else {
                return rdf::StringLiteral(
                        std::string((const char*)node->value.literal.string, node->value.literal.string_len)
                );
            }
        default:
            LOG(INFO) << "Error: unsupported node type " << node->type;
            return rdf::Value();
    }
}


rdf::URI ConvertURI(raptor_term *node) {
    if (node == nullptr) {
        return rdf::URI();
    }

    switch (node->type) {
        case RAPTOR_TERM_TYPE_URI:
            return rdf::URI((const char*)raptor_uri_as_string(node->value.uri));
        default:
            return rdf::URI();
    }
}


rdf::Statement ConvertStatement(raptor_statement *triple) {
    if (triple->graph != nullptr) {
        return rdf::Statement(
                ConvertResource(triple->subject),
                ConvertURI(triple->predicate),
                ConvertValue(triple->object),
                ConvertResource(triple->graph)
        );
    } else {
        return rdf::Statement(
                ConvertResource(triple->subject),
                ConvertURI(triple->predicate),
                ConvertValue(triple->object)
        );

    }
}
}  // namespace raptor
}  // namespace util
}  // namespace marmotta

