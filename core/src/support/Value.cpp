/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
#include "Value.h"
#include <iomanip>
#include <sstream>
#include <stdlib.h>
#ifndef FLT_EPSILON
#define FLT_EPSILON 1.192092896e-07F
#endif
#include<cstring>
PalValue::PalValue(const PalValue &other) : mType(Type::NONE) { *this = other; }

PalValue::~PalValue() { clear(); }

PalValue &PalValue::operator=(const PalValue &other)
{
    if (this != &other)
    {
        reset(other.mType);

        switch (other.mType)
        {
            case Type::BYTE:
                mField.byteVal = other.mField.byteVal;
                break;
            case Type::INTEGER:
                mField.intVal = other.mField.intVal;
                break;
            case Type::FLOAT:
                mField.floatVal = other.mField.floatVal;
                break;
            case Type::DOUBLE:
                mField.doubleVal = other.mField.doubleVal;
                break;
            case Type::BOOLEAN:
                mField.boolVal = other.mField.boolVal;
                break;
            case Type::STRING:
                if (mField.strVal == nullptr)
                {
                    mField.strVal = new std::string();
                }
                *mField.strVal = *other.mField.strVal;
                break;
            case Type::VECTOR:
                if (mField.vectorVal == nullptr)
                {
                    mField.vectorVal = new ValueVector();
                }
                *mField.vectorVal = *other.mField.vectorVal;
                break;
            case Type::MAP:
                if (mField.mapVal == nullptr)
                {
                    mField.mapVal = new ValueMap();
                }
                *mField.mapVal = *other.mField.mapVal;
                break;
            case Type::INT_KEY_MAP:
                if (mField.intKeyMapVal == nullptr)
                {
                    mField.intKeyMapVal = new ValueMapIntKey();
                }
                *mField.intKeyMapVal = *other.mField.intKeyMapVal;
                break;
            default:
                break;
        }
    }
    return *this;
}

std::string PalValue::asString() const
{
    if (mType == Type::STRING)
    {
        return *mField.strVal;
    }

    unsigned int maxSizeOfString = sizeof(mField.doubleVal);
    char buffer[maxSizeOfString + 1];

    switch (mType)
    {
        case Type::BYTE:
            sprintf(buffer, "%c", mField.byteVal);
            break;
        case Type::INTEGER:
            sprintf(buffer, "%d", mField.intVal);
            break;
        case Type::FLOAT:
            sprintf(buffer, "%.7f", mField.floatVal);
            break;
        case Type::DOUBLE:
            sprintf(buffer, "%.16f", mField.doubleVal);
            break;
        case Type::BOOLEAN:
            if (mField.boolVal)
                memcpy(buffer, "true", 5);
            else
                memcpy(buffer, "false", 6);
            break;
        default:
            break;
    }

    std::string ret(buffer);
    return ret;
}

void PalValue::clear()
{
    switch (mType)
    {
        case Type::BYTE:
            mField.byteVal = 0;
            break;
        case Type::INTEGER:
            mField.intVal = 0;
            break;
        case Type::FLOAT:
            mField.floatVal = 0.0f;
            break;
        case Type::DOUBLE:
            mField.doubleVal = 0.0;
            break;
        case Type::BOOLEAN:
            mField.boolVal = false;
            break;
        case Type::STRING:
            GC_SAFE_DELETE(mField.strVal);
            break;
        case Type::VECTOR:
            GC_SAFE_DELETE(mField.vectorVal);
            break;
        case Type::MAP:
            GC_SAFE_DELETE(mField.mapVal);
            break;
        case Type::INT_KEY_MAP:
            GC_SAFE_DELETE(mField.intKeyMapVal);
            break;
        default:
            break;
    }

    mType = Type::NONE;
}

void PalValue::reset(Type type)
{
    if (mType == type) return;

    clear();

    switch (type)
    {
        case Type::STRING:
            mField.strVal = new std::string();
            break;
        case Type::VECTOR:
            mField.vectorVal = new ValueVector();
            break;
        case Type::MAP:
            mField.mapVal = new ValueMap();
            break;
        case Type::INT_KEY_MAP:
            mField.intKeyMapVal = new ValueMapIntKey();
            break;
        default:
            break;
    }

    mType = type;
}
