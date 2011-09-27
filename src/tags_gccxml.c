/*
 * This file is part of tags2db.
 *
 * Tags2db is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Tags2db is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Tags2db.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "global.h"
#include "tags_gccxml.h"
#include <libxml/xmlreader.h>

    Record*
tags_gccxml_read_one_record(InputTagObject* ito)
{
    static xmlTextReaderPtr    xml_reader;
    static bool                first = true;

    int                        i;
    int                        attr_count;
    xmlChar*                   name;
    Record*                    ret;
    

    /* intialize libxml2 when it is the first time to call this function */
    if(first)
    {
        first = false;

        /* open the xmlReader */
        xml_reader = xmlReaderForFd(fileno(ito->input_tag_file), "", NULL, 0);

        if(!xml_reader)
            return NULL;

        xmlTextReaderRead(xml_reader);
    }

    /* skip one node */
    xmlTextReaderRead(xml_reader);

    xmlTextReaderRead(xml_reader);

    /* get the count of attributes */
    attr_count = xmlTextReaderAttributeCount(xml_reader);

    /* malloc the spaces needed to store the fields */
    ret = (Record*) malloc(sizeof(Record));
    ret->number = attr_count + 1;
    ret->fields_name = (char**) malloc(sizeof(char*) * ret->number);
    ret->data = (Variant*) malloc(sizeof(Variant) * ret->number);

    /* the type field */
    ret->fields_name[0] = strdup("type");
    ret->data[0].type = VARIANT_TYPE_STRING;
    name = xmlTextReaderName(xml_reader);
    if(!name)
        return NULL;
    ret->data[0].data.string_data = strdup(name);
    xmlFree(name);

    /* deal with each attribute */
    for(i = 0; i < attr_count; ++ i)
    {
        xmlChar*            value;

        xmlTextReaderMoveToAttributeNo(xml_reader, i);
        name = xmlTextReaderName(xml_reader);
        value = xmlTextReaderValue(xml_reader);

        if((!name) || (!value))
            return NULL;

        ret->fields_name[i + 1] = strdup(name);
        ret->data[i + 1].type = VARIANT_TYPE_STRING;
        ret->data[i + 1].data.string_data = strdup(value);

        xmlFree(name);
        xmlFree(value);
    }
    xmlTextReaderMoveToElement(xml_reader);

    return ret;
}
