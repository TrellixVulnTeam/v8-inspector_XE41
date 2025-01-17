// File is generated by Source/core/inspector/CodeGeneratorInspector.py

// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.


#include "config.h"

#include "InspectorTypeBuilder.h"
#include "wtf/text/CString.h"

namespace blink {

namespace TypeBuilder {

const char* const enum_constant_values[] = {
    "Document",
    "Stylesheet",
    "Image",
    "Media",
    "Font",
    "Script",
    "TextTrack",
    "XHR",
    "WebSocket",
    "Other",
    "object",
    "function",
    "undefined",
    "string",
    "number",
    "boolean",
    "symbol",
    "array",
    "null",
    "node",
    "regexp",
    "date",
    "map",
    "set",
    "iterator",
    "generator",
    "error",
    "accessor",
    "xml",
    "javascript",
    "network",
    "console-api",
    "storage",
    "appcache",
    "rendering",
    "security",
    "other",
    "deprecation",
    "log",
    "warning",
    "debug",
    "info",
    "revokedError",
    "dir",
    "dirxml",
    "table",
    "trace",
    "clear",
    "startGroup",
    "startGroupCollapsed",
    "endGroup",
    "assert",
    "profile",
    "profileEnd",
    "parser",
    "script",
    "first-line",
    "first-letter",
    "before",
    "after",
    "backdrop",
    "selection",
    "first-line-inherited",
    "scrollbar",
    "scrollbar-thumb",
    "scrollbar-button",
    "scrollbar-track",
    "scrollbar-track-piece",
    "scrollbar-corner",
    "resizer",
    "input-list-button",
    "user-agent",
    "author",
    "injected",
    "inspector",
    "regular",
    "mediaRule",
    "importRule",
    "linkedSheet",
    "inlineSheet",
    "running",
    "suspended",
    "closed",
    "global",
    "local",
    "with",
    "closure",
    "catch",
    "block",
    "pending",
    "resolved",
    "rejected",
    "subtree-modified",
    "attribute-modified",
    "node-removed",
    "stopped",
    "starting",
    "stopping",
    "new",
    "installing",
    "installed",
    "activating",
    "activated",
    "redundant",
    "touchPressed",
    "touchReleased",
    "touchMoved",
    "touchStationary",
    "touchCancelled",
    "default",
    "touch",
    "mouse",
    "RepaintsOnScroll",
    "TouchEventHandler",
    "WheelEventHandler",
    "CSSTransition",
    "CSSAnimation",
    "WebAnimation",
    "tristate",
    "booleanOrUndefined",
    "idref",
    "idrefList",
    "integer",
    "token",
    "tokenList",
    "domRelation",
    "role",
    "internalRole",
    "attribute",
    "implicit",
    "style",
    "disabled",
    "hidden",
    "hiddenRoot",
    "invalid",
    "live",
    "atomic",
    "relevant",
    "busy",
    "root",
    "autocomplete",
    "haspopup",
    "level",
    "multiselectable",
    "orientation",
    "multiline",
    "readonly",
    "required",
    "valuemin",
    "valuemax",
    "valuetext",
    "checked",
    "expanded",
    "pressed",
    "selected",
    "activedescendant",
    "flowto",
    "controls",
    "describedby",
    "labelledby",
    "owns",
    "DOM",
    "EventListener",
    "exception",
    "CSPViolation",
    "debugCommand",
    "promiseRejection",
    "AsyncOperation",
    "update",
    "gc"
};

String getEnumConstantValue(int code) {
    return enum_constant_values[code];
}

} // namespace TypeBuilder

const char TypeBuilder::Network::Response::Url[] = "url";
const char TypeBuilder::Network::Response::Status[] = "status";
const char TypeBuilder::Network::Response::StatusText[] = "statusText";
const char TypeBuilder::Network::Response::Headers[] = "headers";
const char TypeBuilder::Network::Response::MimeType[] = "mimeType";
const char TypeBuilder::Network::Response::ConnectionReused[] = "connectionReused";
const char TypeBuilder::Network::Response::ConnectionId[] = "connectionId";
const char TypeBuilder::Network::Response::EncodedDataLength[] = "encodedDataLength";
const char TypeBuilder::Network::Response::HeadersText[] = "headersText";
const char TypeBuilder::Network::Response::RequestHeaders[] = "requestHeaders";
const char TypeBuilder::Network::Response::RequestHeadersText[] = "requestHeadersText";
const char TypeBuilder::Network::Response::RemoteIPAddress[] = "remoteIPAddress";
const char TypeBuilder::Network::Response::RemotePort[] = "remotePort";
const char TypeBuilder::Network::Response::FromDiskCache[] = "fromDiskCache";
const char TypeBuilder::Network::Response::FromServiceWorker[] = "fromServiceWorker";
const char TypeBuilder::Network::Response::Timing[] = "timing";
const char TypeBuilder::Network::Response::Protocol[] = "protocol";
const char TypeBuilder::CSS::CSSProperty::Name[] = "name";
const char TypeBuilder::CSS::CSSProperty::Value[] = "value";
const char TypeBuilder::CSS::CSSProperty::Important[] = "important";
const char TypeBuilder::CSS::CSSProperty::Implicit[] = "implicit";
const char TypeBuilder::CSS::CSSProperty::Text[] = "text";
const char TypeBuilder::CSS::CSSProperty::ParsedOk[] = "parsedOk";
const char TypeBuilder::CSS::CSSProperty::Disabled[] = "disabled";
const char TypeBuilder::CSS::CSSProperty::Range[] = "range";


#if ENABLE(ASSERT)

void TypeBuilder::RuntimeCastHelper::assertAny(JSONValue*)
{
    // No-op.
}


void TypeBuilder::RuntimeCastHelper::assertInt(JSONValue* value)
{
    double v;
    bool castRes = value->asNumber(&v);
    ASSERT_UNUSED(castRes, castRes);
    ASSERT(static_cast<double>(static_cast<int>(v)) == v);
}

void TypeBuilder::Runtime::RemoteObject::Type::assertCorrectValue(JSONValue* value)
{
    WTF::String s;
    bool cast_res = value->asString(&s);
    ASSERT(cast_res);
    ASSERT(s == "object" || s == "function" || s == "undefined" || s == "string" || s == "number" || s == "boolean" || s == "symbol");
}


void TypeBuilder::Runtime::RemoteObject::Subtype::assertCorrectValue(JSONValue* value)
{
    WTF::String s;
    bool cast_res = value->asString(&s);
    ASSERT(cast_res);
    ASSERT(s == "array" || s == "null" || s == "node" || s == "regexp" || s == "date" || s == "map" || s == "set" || s == "iterator" || s == "generator" || s == "error");
}


void TypeBuilder::Runtime::RemoteObject::assertCorrectValue(JSONValue* value)
{
    RefPtr<JSONObject> object;
    bool castRes = value->asObject(&object);
    ASSERT_UNUSED(castRes, castRes);
    {
        JSONObject::iterator typePos;
        typePos = object->find("type");
        ASSERT(typePos != object->end());
        Type::assertCorrectValue(typePos->value.get());
    }
    int foundPropertiesCount = 1;
    {
        JSONObject::iterator subtypePos;
        subtypePos = object->find("subtype");
        if (subtypePos != object->end()) {
            Subtype::assertCorrectValue(subtypePos->value.get());
            ++foundPropertiesCount;
        }
    }
    {
        JSONObject::iterator classNamePos;
        classNamePos = object->find("className");
        if (classNamePos != object->end()) {
            RuntimeCastHelper::assertType<JSONValue::TypeString>(classNamePos->value.get());
            ++foundPropertiesCount;
        }
    }
    {
        JSONObject::iterator valuePos;
        valuePos = object->find("value");
        if (valuePos != object->end()) {
            RuntimeCastHelper::assertAny(valuePos->value.get());
            ++foundPropertiesCount;
        }
    }
    {
        JSONObject::iterator descriptionPos;
        descriptionPos = object->find("description");
        if (descriptionPos != object->end()) {
            RuntimeCastHelper::assertType<JSONValue::TypeString>(descriptionPos->value.get());
            ++foundPropertiesCount;
        }
    }
    {
        JSONObject::iterator objectIdPos;
        objectIdPos = object->find("objectId");
        if (objectIdPos != object->end()) {
            RuntimeCastHelper::assertType<JSONValue::TypeString>(objectIdPos->value.get());
            ++foundPropertiesCount;
        }
    }
    {
        JSONObject::iterator previewPos;
        previewPos = object->find("preview");
        if (previewPos != object->end()) {
            TypeBuilder::Runtime::ObjectPreview::assertCorrectValue(previewPos->value.get());
            ++foundPropertiesCount;
        }
    }
    {
        JSONObject::iterator customPreviewPos;
        customPreviewPos = object->find("customPreview");
        if (customPreviewPos != object->end()) {
            TypeBuilder::Runtime::CustomPreview::assertCorrectValue(customPreviewPos->value.get());
            ++foundPropertiesCount;
        }
    }
    if (foundPropertiesCount != object->size()) {
      FATAL("Unexpected properties in object: %s\n", object->toJSONString().ascii().data());
    }
}


void TypeBuilder::Runtime::CustomPreview::assertCorrectValue(JSONValue* value)
{
    RefPtr<JSONObject> object;
    bool castRes = value->asObject(&object);
    ASSERT_UNUSED(castRes, castRes);
    {
        JSONObject::iterator headerPos;
        headerPos = object->find("header");
        ASSERT(headerPos != object->end());
        RuntimeCastHelper::assertType<JSONValue::TypeString>(headerPos->value.get());
    }
    {
        JSONObject::iterator hasBodyPos;
        hasBodyPos = object->find("hasBody");
        ASSERT(hasBodyPos != object->end());
        RuntimeCastHelper::assertType<JSONValue::TypeBoolean>(hasBodyPos->value.get());
    }
    {
        JSONObject::iterator formatterObjectIdPos;
        formatterObjectIdPos = object->find("formatterObjectId");
        ASSERT(formatterObjectIdPos != object->end());
        RuntimeCastHelper::assertType<JSONValue::TypeString>(formatterObjectIdPos->value.get());
    }
    int foundPropertiesCount = 3;
    {
        JSONObject::iterator configObjectIdPos;
        configObjectIdPos = object->find("configObjectId");
        if (configObjectIdPos != object->end()) {
            RuntimeCastHelper::assertType<JSONValue::TypeString>(configObjectIdPos->value.get());
            ++foundPropertiesCount;
        }
    }
    if (foundPropertiesCount != object->size()) {
      FATAL("Unexpected properties in object: %s\n", object->toJSONString().ascii().data());
    }
}


void TypeBuilder::Runtime::ObjectPreview::Type::assertCorrectValue(JSONValue* value)
{
    WTF::String s;
    bool cast_res = value->asString(&s);
    ASSERT(cast_res);
    ASSERT(s == "object" || s == "function" || s == "undefined" || s == "string" || s == "number" || s == "boolean" || s == "symbol");
}


void TypeBuilder::Runtime::ObjectPreview::Subtype::assertCorrectValue(JSONValue* value)
{
    WTF::String s;
    bool cast_res = value->asString(&s);
    ASSERT(cast_res);
    ASSERT(s == "array" || s == "null" || s == "node" || s == "regexp" || s == "date" || s == "map" || s == "set" || s == "iterator" || s == "generator" || s == "error");
}


void TypeBuilder::Runtime::ObjectPreview::assertCorrectValue(JSONValue* value)
{
    RefPtr<JSONObject> object;
    bool castRes = value->asObject(&object);
    ASSERT_UNUSED(castRes, castRes);
    {
        JSONObject::iterator typePos;
        typePos = object->find("type");
        ASSERT(typePos != object->end());
        Type::assertCorrectValue(typePos->value.get());
    }
    {
        JSONObject::iterator losslessPos;
        losslessPos = object->find("lossless");
        ASSERT(losslessPos != object->end());
        RuntimeCastHelper::assertType<JSONValue::TypeBoolean>(losslessPos->value.get());
    }
    {
        JSONObject::iterator overflowPos;
        overflowPos = object->find("overflow");
        ASSERT(overflowPos != object->end());
        RuntimeCastHelper::assertType<JSONValue::TypeBoolean>(overflowPos->value.get());
    }
    {
        JSONObject::iterator propertiesPos;
        propertiesPos = object->find("properties");
        ASSERT(propertiesPos != object->end());
        TypeBuilder::Array<TypeBuilder::Runtime::PropertyPreview>::assertCorrectValue(propertiesPos->value.get());
    }
    int foundPropertiesCount = 4;
    {
        JSONObject::iterator subtypePos;
        subtypePos = object->find("subtype");
        if (subtypePos != object->end()) {
            Subtype::assertCorrectValue(subtypePos->value.get());
            ++foundPropertiesCount;
        }
    }
    {
        JSONObject::iterator descriptionPos;
        descriptionPos = object->find("description");
        if (descriptionPos != object->end()) {
            RuntimeCastHelper::assertType<JSONValue::TypeString>(descriptionPos->value.get());
            ++foundPropertiesCount;
        }
    }
    {
        JSONObject::iterator entriesPos;
        entriesPos = object->find("entries");
        if (entriesPos != object->end()) {
            TypeBuilder::Array<TypeBuilder::Runtime::EntryPreview>::assertCorrectValue(entriesPos->value.get());
            ++foundPropertiesCount;
        }
    }
    if (foundPropertiesCount != object->size()) {
      FATAL("Unexpected properties in object: %s\n", object->toJSONString().ascii().data());
    }
}


void TypeBuilder::Runtime::PropertyPreview::Type::assertCorrectValue(JSONValue* value)
{
    WTF::String s;
    bool cast_res = value->asString(&s);
    ASSERT(cast_res);
    ASSERT(s == "object" || s == "function" || s == "undefined" || s == "string" || s == "number" || s == "boolean" || s == "symbol" || s == "accessor");
}


void TypeBuilder::Runtime::PropertyPreview::Subtype::assertCorrectValue(JSONValue* value)
{
    WTF::String s;
    bool cast_res = value->asString(&s);
    ASSERT(cast_res);
    ASSERT(s == "array" || s == "null" || s == "node" || s == "regexp" || s == "date" || s == "map" || s == "set" || s == "iterator" || s == "generator" || s == "error");
}


void TypeBuilder::Runtime::PropertyPreview::assertCorrectValue(JSONValue* value)
{
    RefPtr<JSONObject> object;
    bool castRes = value->asObject(&object);
    ASSERT_UNUSED(castRes, castRes);
    {
        JSONObject::iterator namePos;
        namePos = object->find("name");
        ASSERT(namePos != object->end());
        RuntimeCastHelper::assertType<JSONValue::TypeString>(namePos->value.get());
    }
    {
        JSONObject::iterator typePos;
        typePos = object->find("type");
        ASSERT(typePos != object->end());
        Type::assertCorrectValue(typePos->value.get());
    }
    int foundPropertiesCount = 2;
    {
        JSONObject::iterator valuePos;
        valuePos = object->find("value");
        if (valuePos != object->end()) {
            RuntimeCastHelper::assertType<JSONValue::TypeString>(valuePos->value.get());
            ++foundPropertiesCount;
        }
    }
    {
        JSONObject::iterator valuePreviewPos;
        valuePreviewPos = object->find("valuePreview");
        if (valuePreviewPos != object->end()) {
            TypeBuilder::Runtime::ObjectPreview::assertCorrectValue(valuePreviewPos->value.get());
            ++foundPropertiesCount;
        }
    }
    {
        JSONObject::iterator subtypePos;
        subtypePos = object->find("subtype");
        if (subtypePos != object->end()) {
            Subtype::assertCorrectValue(subtypePos->value.get());
            ++foundPropertiesCount;
        }
    }
    if (foundPropertiesCount != object->size()) {
      FATAL("Unexpected properties in object: %s\n", object->toJSONString().ascii().data());
    }
}


void TypeBuilder::Runtime::EntryPreview::assertCorrectValue(JSONValue* value)
{
    RefPtr<JSONObject> object;
    bool castRes = value->asObject(&object);
    ASSERT_UNUSED(castRes, castRes);
    {
        JSONObject::iterator valuePos;
        valuePos = object->find("value");
        ASSERT(valuePos != object->end());
        TypeBuilder::Runtime::ObjectPreview::assertCorrectValue(valuePos->value.get());
    }
    int foundPropertiesCount = 1;
    {
        JSONObject::iterator keyPos;
        keyPos = object->find("key");
        if (keyPos != object->end()) {
            TypeBuilder::Runtime::ObjectPreview::assertCorrectValue(keyPos->value.get());
            ++foundPropertiesCount;
        }
    }
    if (foundPropertiesCount != object->size()) {
      FATAL("Unexpected properties in object: %s\n", object->toJSONString().ascii().data());
    }
}


void TypeBuilder::Runtime::PropertyDescriptor::assertCorrectValue(JSONValue* value)
{
    RefPtr<JSONObject> object;
    bool castRes = value->asObject(&object);
    ASSERT_UNUSED(castRes, castRes);
    {
        JSONObject::iterator namePos;
        namePos = object->find("name");
        ASSERT(namePos != object->end());
        RuntimeCastHelper::assertType<JSONValue::TypeString>(namePos->value.get());
    }
    {
        JSONObject::iterator configurablePos;
        configurablePos = object->find("configurable");
        ASSERT(configurablePos != object->end());
        RuntimeCastHelper::assertType<JSONValue::TypeBoolean>(configurablePos->value.get());
    }
    {
        JSONObject::iterator enumerablePos;
        enumerablePos = object->find("enumerable");
        ASSERT(enumerablePos != object->end());
        RuntimeCastHelper::assertType<JSONValue::TypeBoolean>(enumerablePos->value.get());
    }
    int foundPropertiesCount = 3;
    {
        JSONObject::iterator valuePos;
        valuePos = object->find("value");
        if (valuePos != object->end()) {
            TypeBuilder::Runtime::RemoteObject::assertCorrectValue(valuePos->value.get());
            ++foundPropertiesCount;
        }
    }
    {
        JSONObject::iterator writablePos;
        writablePos = object->find("writable");
        if (writablePos != object->end()) {
            RuntimeCastHelper::assertType<JSONValue::TypeBoolean>(writablePos->value.get());
            ++foundPropertiesCount;
        }
    }
    {
        JSONObject::iterator getPos;
        getPos = object->find("get");
        if (getPos != object->end()) {
            TypeBuilder::Runtime::RemoteObject::assertCorrectValue(getPos->value.get());
            ++foundPropertiesCount;
        }
    }
    {
        JSONObject::iterator setPos;
        setPos = object->find("set");
        if (setPos != object->end()) {
            TypeBuilder::Runtime::RemoteObject::assertCorrectValue(setPos->value.get());
            ++foundPropertiesCount;
        }
    }
    {
        JSONObject::iterator wasThrownPos;
        wasThrownPos = object->find("wasThrown");
        if (wasThrownPos != object->end()) {
            RuntimeCastHelper::assertType<JSONValue::TypeBoolean>(wasThrownPos->value.get());
            ++foundPropertiesCount;
        }
    }
    {
        JSONObject::iterator isOwnPos;
        isOwnPos = object->find("isOwn");
        if (isOwnPos != object->end()) {
            RuntimeCastHelper::assertType<JSONValue::TypeBoolean>(isOwnPos->value.get());
            ++foundPropertiesCount;
        }
    }
    {
        JSONObject::iterator symbolPos;
        symbolPos = object->find("symbol");
        if (symbolPos != object->end()) {
            TypeBuilder::Runtime::RemoteObject::assertCorrectValue(symbolPos->value.get());
            ++foundPropertiesCount;
        }
    }
    if (foundPropertiesCount != object->size()) {
      FATAL("Unexpected properties in object: %s\n", object->toJSONString().ascii().data());
    }
}


void TypeBuilder::Runtime::InternalPropertyDescriptor::assertCorrectValue(JSONValue* value)
{
    RefPtr<JSONObject> object;
    bool castRes = value->asObject(&object);
    ASSERT_UNUSED(castRes, castRes);
    {
        JSONObject::iterator namePos;
        namePos = object->find("name");
        ASSERT(namePos != object->end());
        RuntimeCastHelper::assertType<JSONValue::TypeString>(namePos->value.get());
    }
    int foundPropertiesCount = 1;
    {
        JSONObject::iterator valuePos;
        valuePos = object->find("value");
        if (valuePos != object->end()) {
            TypeBuilder::Runtime::RemoteObject::assertCorrectValue(valuePos->value.get());
            ++foundPropertiesCount;
        }
    }
    if (foundPropertiesCount != object->size()) {
      FATAL("Unexpected properties in object: %s\n", object->toJSONString().ascii().data());
    }
}


void TypeBuilder::Debugger::Location::assertCorrectValue(JSONValue* value)
{
    RefPtr<JSONObject> object;
    bool castRes = value->asObject(&object);
    ASSERT_UNUSED(castRes, castRes);
    {
        JSONObject::iterator scriptIdPos;
        scriptIdPos = object->find("scriptId");
        ASSERT(scriptIdPos != object->end());
        RuntimeCastHelper::assertType<JSONValue::TypeString>(scriptIdPos->value.get());
    }
    {
        JSONObject::iterator lineNumberPos;
        lineNumberPos = object->find("lineNumber");
        ASSERT(lineNumberPos != object->end());
        RuntimeCastHelper::assertInt(lineNumberPos->value.get());
    }
    int foundPropertiesCount = 2;
    {
        JSONObject::iterator columnNumberPos;
        columnNumberPos = object->find("columnNumber");
        if (columnNumberPos != object->end()) {
            RuntimeCastHelper::assertInt(columnNumberPos->value.get());
            ++foundPropertiesCount;
        }
    }
    if (foundPropertiesCount != object->size()) {
      FATAL("Unexpected properties in object: %s\n", object->toJSONString().ascii().data());
    }
}


void TypeBuilder::Debugger::FunctionDetails::assertCorrectValue(JSONValue* value)
{
    RefPtr<JSONObject> object;
    bool castRes = value->asObject(&object);
    ASSERT_UNUSED(castRes, castRes);
    {
        JSONObject::iterator functionNamePos;
        functionNamePos = object->find("functionName");
        ASSERT(functionNamePos != object->end());
        RuntimeCastHelper::assertType<JSONValue::TypeString>(functionNamePos->value.get());
    }
    {
        JSONObject::iterator isGeneratorPos;
        isGeneratorPos = object->find("isGenerator");
        ASSERT(isGeneratorPos != object->end());
        RuntimeCastHelper::assertType<JSONValue::TypeBoolean>(isGeneratorPos->value.get());
    }
    int foundPropertiesCount = 2;
    {
        JSONObject::iterator locationPos;
        locationPos = object->find("location");
        if (locationPos != object->end()) {
            TypeBuilder::Debugger::Location::assertCorrectValue(locationPos->value.get());
            ++foundPropertiesCount;
        }
    }
    {
        JSONObject::iterator scopeChainPos;
        scopeChainPos = object->find("scopeChain");
        if (scopeChainPos != object->end()) {
            TypeBuilder::Array<TypeBuilder::Debugger::Scope>::assertCorrectValue(scopeChainPos->value.get());
            ++foundPropertiesCount;
        }
    }
    if (foundPropertiesCount != object->size()) {
      FATAL("Unexpected properties in object: %s\n", object->toJSONString().ascii().data());
    }
}


void TypeBuilder::Debugger::GeneratorObjectDetails::Status::assertCorrectValue(JSONValue* value)
{
    WTF::String s;
    bool cast_res = value->asString(&s);
    ASSERT(cast_res);
    ASSERT(s == "running" || s == "suspended" || s == "closed");
}


void TypeBuilder::Debugger::GeneratorObjectDetails::assertCorrectValue(JSONValue* value)
{
    RefPtr<JSONObject> object;
    bool castRes = value->asObject(&object);
    ASSERT_UNUSED(castRes, castRes);
    {
        JSONObject::iterator functionPos;
        functionPos = object->find("function");
        ASSERT(functionPos != object->end());
        TypeBuilder::Runtime::RemoteObject::assertCorrectValue(functionPos->value.get());
    }
    {
        JSONObject::iterator functionNamePos;
        functionNamePos = object->find("functionName");
        ASSERT(functionNamePos != object->end());
        RuntimeCastHelper::assertType<JSONValue::TypeString>(functionNamePos->value.get());
    }
    {
        JSONObject::iterator statusPos;
        statusPos = object->find("status");
        ASSERT(statusPos != object->end());
        Status::assertCorrectValue(statusPos->value.get());
    }
    int foundPropertiesCount = 3;
    {
        JSONObject::iterator locationPos;
        locationPos = object->find("location");
        if (locationPos != object->end()) {
            TypeBuilder::Debugger::Location::assertCorrectValue(locationPos->value.get());
            ++foundPropertiesCount;
        }
    }
    if (foundPropertiesCount != object->size()) {
      FATAL("Unexpected properties in object: %s\n", object->toJSONString().ascii().data());
    }
}


void TypeBuilder::Debugger::CollectionEntry::assertCorrectValue(JSONValue* value)
{
    RefPtr<JSONObject> object;
    bool castRes = value->asObject(&object);
    ASSERT_UNUSED(castRes, castRes);
    {
        JSONObject::iterator valuePos;
        valuePos = object->find("value");
        ASSERT(valuePos != object->end());
        TypeBuilder::Runtime::RemoteObject::assertCorrectValue(valuePos->value.get());
    }
    int foundPropertiesCount = 1;
    {
        JSONObject::iterator keyPos;
        keyPos = object->find("key");
        if (keyPos != object->end()) {
            TypeBuilder::Runtime::RemoteObject::assertCorrectValue(keyPos->value.get());
            ++foundPropertiesCount;
        }
    }
    if (foundPropertiesCount != object->size()) {
      FATAL("Unexpected properties in object: %s\n", object->toJSONString().ascii().data());
    }
}


void TypeBuilder::Debugger::CallFrame::assertCorrectValue(JSONValue* value)
{
    RefPtr<JSONObject> object;
    bool castRes = value->asObject(&object);
    ASSERT_UNUSED(castRes, castRes);
    {
        JSONObject::iterator callFrameIdPos;
        callFrameIdPos = object->find("callFrameId");
        ASSERT(callFrameIdPos != object->end());
        RuntimeCastHelper::assertType<JSONValue::TypeString>(callFrameIdPos->value.get());
    }
    {
        JSONObject::iterator functionNamePos;
        functionNamePos = object->find("functionName");
        ASSERT(functionNamePos != object->end());
        RuntimeCastHelper::assertType<JSONValue::TypeString>(functionNamePos->value.get());
    }
    {
        JSONObject::iterator locationPos;
        locationPos = object->find("location");
        ASSERT(locationPos != object->end());
        TypeBuilder::Debugger::Location::assertCorrectValue(locationPos->value.get());
    }
    {
        JSONObject::iterator scopeChainPos;
        scopeChainPos = object->find("scopeChain");
        ASSERT(scopeChainPos != object->end());
        TypeBuilder::Array<TypeBuilder::Debugger::Scope>::assertCorrectValue(scopeChainPos->value.get());
    }
    {
        JSONObject::iterator thisPos;
        thisPos = object->find("this");
        ASSERT(thisPos != object->end());
        TypeBuilder::Runtime::RemoteObject::assertCorrectValue(thisPos->value.get());
    }
    int foundPropertiesCount = 5;
    {
        JSONObject::iterator functionLocationPos;
        functionLocationPos = object->find("functionLocation");
        if (functionLocationPos != object->end()) {
            TypeBuilder::Debugger::Location::assertCorrectValue(functionLocationPos->value.get());
            ++foundPropertiesCount;
        }
    }
    {
        JSONObject::iterator returnValuePos;
        returnValuePos = object->find("returnValue");
        if (returnValuePos != object->end()) {
            TypeBuilder::Runtime::RemoteObject::assertCorrectValue(returnValuePos->value.get());
            ++foundPropertiesCount;
        }
    }
    if (foundPropertiesCount != object->size()) {
      FATAL("Unexpected properties in object: %s\n", object->toJSONString().ascii().data());
    }
}


void TypeBuilder::Debugger::Scope::Type::assertCorrectValue(JSONValue* value)
{
    WTF::String s;
    bool cast_res = value->asString(&s);
    ASSERT(cast_res);
    ASSERT(s == "global" || s == "local" || s == "with" || s == "closure" || s == "catch" || s == "block" || s == "script");
}


void TypeBuilder::Debugger::Scope::assertCorrectValue(JSONValue* value)
{
    RefPtr<JSONObject> object;
    bool castRes = value->asObject(&object);
    ASSERT_UNUSED(castRes, castRes);
    {
        JSONObject::iterator typePos;
        typePos = object->find("type");
        ASSERT(typePos != object->end());
        Type::assertCorrectValue(typePos->value.get());
    }
    {
        JSONObject::iterator objectPos;
        objectPos = object->find("object");
        ASSERT(objectPos != object->end());
        TypeBuilder::Runtime::RemoteObject::assertCorrectValue(objectPos->value.get());
    }
    int foundPropertiesCount = 2;
    if (foundPropertiesCount != object->size()) {
      FATAL("Unexpected properties in object: %s\n", object->toJSONString().ascii().data());
    }
}




#endif // ENABLE(ASSERT)

} // namespace blink

