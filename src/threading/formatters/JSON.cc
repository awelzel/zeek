// See the file "COPYING" in the main distribution directory for copyright.

#include "zeek-config.h"

#ifndef __STDC_LIMIT_MACROS
#define __STDC_LIMIT_MACROS
#endif

#include <sstream>
#include <errno.h>
#include <math.h>
#include <stdint.h>

#define RAPIDJSON_HAS_STDSTRING 1
#include "3rdparty/rapidjson/include/rapidjson/document.h"
#include "3rdparty/rapidjson/include/rapidjson/stringbuffer.h"
#include "3rdparty/rapidjson/include/rapidjson/writer.h"

#include "JSON.h"

using namespace threading::formatter;

template<typename OutputStream,
		 typename SourceEncoding = rapidjson::UTF8<>,
		 typename TargetEncoding = rapidjson::UTF8<>,
		 typename Allocator = rapidjson::CrtAllocator,
		 unsigned writeFlags = rapidjson::kWriteDefaultFlags | rapidjson::kWriteNanAndInfFlag>
using JsonWriter = rapidjson::Writer<OutputStream, SourceEncoding, TargetEncoding, Allocator, writeFlags>;

JSON::JSON(MsgThread* t, TimeFormat tf) : Formatter(t), surrounding_braces(true)
	{
	timestamps = tf;
	}

JSON::~JSON()
	{
	}

bool JSON::Describe(ODesc* desc, int num_fields, const Field* const * fields,
                    Value** vals) const
	{
	rapidjson::Document doc;
	rapidjson::Value j(rapidjson::kObjectType);

	for ( int i = 0; i < num_fields; i++ )
		{
		if ( vals[i]->present )
			{
			rapidjson::Value new_entry = BuildJSON(doc, vals[i]);
			if ( new_entry.IsNull() )
				return false;

			rapidjson::Value key(fields[i]->name, doc.GetAllocator());
			j.AddMember(key, new_entry, doc.GetAllocator());
			}
		}

	rapidjson::StringBuffer buffer;
	JsonWriter<rapidjson::StringBuffer> writer(buffer);
	j.Accept(writer);
	desc->Add(buffer.GetString());

	return true;
	}

bool JSON::Describe(ODesc* desc, Value* val, const string& name) const
	{
	if ( desc->IsBinary() )
		{
		GetThread()->Error("json formatter: binary format not supported");
		return false;
		}

	if ( ! val->present )
		return true;

	rapidjson::Document doc;
	rapidjson::Value j = BuildJSON(doc, val, name);
	if ( j.IsNull() )
		return false;

	rapidjson::StringBuffer buffer;
	JsonWriter<rapidjson::StringBuffer> writer(buffer);
	j.Accept(writer);
	desc->Add(buffer.GetString());
	return true;
	}

threading::Value* JSON::ParseValue(const string& s, const string& name, TypeTag type, TypeTag subtype) const
	{
	GetThread()->Error("JSON formatter does not support parsing yet.");
	return nullptr;
	}

rapidjson::Value JSON::BuildJSON(rapidjson::Document& doc, Value* val, const string& name) const
	{
	rapidjson::Value j;

	// If the value wasn't set, return the null value.
	if ( ! val->present )
		return j;

	switch ( val->type )
		{
		case TYPE_BOOL:
			j.SetBool(val->val.int_val != 0);
			break;

		case TYPE_INT:
			j.SetInt(val->val.int_val);
			break;

		case TYPE_COUNT:
		case TYPE_COUNTER:
			j.SetUint(val->val.uint_val);
			break;

		case TYPE_PORT:
			j.SetUint(val->val.port_val.port);
			break;

		case TYPE_SUBNET:
			j.SetString(Formatter::Render(val->val.subnet_val), doc.GetAllocator());
			break;

		case TYPE_ADDR:
			j.SetString(Formatter::Render(val->val.addr_val), doc.GetAllocator());
			break;

		case TYPE_DOUBLE:
		case TYPE_INTERVAL:
			j.SetDouble(val->val.double_val);
			break;

		case TYPE_TIME:
			{
			if ( timestamps == TS_ISO8601 )
				{
				char buffer[40];
				char buffer2[40];
				time_t the_time = time_t(floor(val->val.double_val));
				struct tm t;

				if ( ! gmtime_r(&the_time, &t) ||
				     ! strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%S", &t) )
					{
					GetThread()->Error(GetThread()->Fmt("json formatter: failure getting time: (%lf)", val->val.double_val));
					// This was a failure, doesn't really matter what gets put here
					// but it should probably stand out...
					j.SetString("2000-01-01T00:00:00.000000", doc.GetAllocator());
					}
				else
					{
					double integ;
					double frac = modf(val->val.double_val, &integ);

					if ( frac < 0 )
						frac += 1;

					snprintf(buffer2, sizeof(buffer2), "%s.%06.0fZ", buffer, fabs(frac) * 1000000);
					j.SetString(buffer2, strlen(buffer2), doc.GetAllocator());
					}
				}

			else if ( timestamps == TS_EPOCH )
				j.SetDouble(val->val.double_val);

			else if ( timestamps == TS_MILLIS )
				{
				// ElasticSearch uses milliseconds for timestamps
				j.SetUint64((uint64_t) (val->val.double_val * 1000));
				}

			break;
			}

		case TYPE_ENUM:
		case TYPE_STRING:
		case TYPE_FILE:
		case TYPE_FUNC:
			{
			j.SetString(json_escape_utf8(string(val->val.string_val.data, val->val.string_val.length)), doc.GetAllocator());
			break;
			}

		case TYPE_TABLE:
			{
 			j = rapidjson::Value(rapidjson::kArrayType);

			for ( int idx = 0; idx < val->val.set_val.size; idx++ )
				j.PushBack(BuildJSON(doc, val->val.set_val.vals[idx]), doc.GetAllocator());

			break;
			}

		case TYPE_VECTOR:
			{
 			j = rapidjson::Value(rapidjson::kArrayType);

			for ( int idx = 0; idx < val->val.vector_val.size; idx++ )
				j.PushBack(BuildJSON(doc, val->val.vector_val.vals[idx]), doc.GetAllocator());

			break;
			}

		default:
			break;
		}

	if ( ! name.empty() && ! j.IsNull() )
		{
		rapidjson::Value j2(rapidjson::kObjectType);
		rapidjson::Value key(name, doc.GetAllocator());
		j2.AddMember(key, j, doc.GetAllocator());
		return j2;
		}

	return j;
	}
