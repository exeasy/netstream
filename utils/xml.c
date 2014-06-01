#include <libxml/tree.h>
#include <libxml/parser.h>
#include "xml.h"


int xml_init()
{
	xmlInitParser();
	return 0;
}

int xml_close()
{
	xmlCleanupParser();
}

xmlDocPtr create_xml_doc()
{
	xmlDocPtr	doc = NULL;

	doc = xmlNewDoc( BAD_CAST "1.0" );

	return doc;
}

/*
	add root node
 */

xmlNodePtr create_xml_root_node( xmlDocPtr doc, char *name )
{
	xmlNodePtr	root_node = NULL;

	root_node = xmlNewNode( NULL, BAD_CAST name );
	xmlDocSetRootElement( doc, root_node );

	return root_node;
}

/*
	add child node
 */

xmlNodePtr add_xml_child( xmlNodePtr node, char *name, char *value )
{
	xmlNodePtr	this_node = NULL;
	this_node = xmlNewChild( node, NULL, BAD_CAST name, value );

	return this_node;
}

/*
	add node's attribute
 */

int add_xml_child_prop( xmlNodePtr node, char *name, char *value )
{
	xmlNewProp( node, BAD_CAST name, BAD_CAST value );
	return 0;
}

/*
 *  get node atrribute by node name 
 */
xmlChar* get_attr_by_name(xmlDocPtr doc, xmlNodePtr node, const xmlChar *name)
{
	if( !xmlHasProp(node, name) )
		return NULL;
	xmlAttrPtr attrPtr = node->properties;
	while(attrPtr != NULL)
	{
		if ( !xmlStrcmp(attrPtr->name, name) )
		{
			xmlChar *szAttr = xmlGetProp( node, name);
			return szAttr;
		}
		attrPtr = attrPtr->next;
	}
	return NULL;
}

/* get node value by node name */
xmlChar *get_value_by_name(xmlDocPtr doc, xmlNodePtr parent, const xmlChar *name)
{
	xmlNodePtr node = NULL;
	for ( node = parent->children; node != NULL; node = node->next)
	{
		if( xmlStrcmp(node->name, name) == 0 )
		{
			return xmlNodeListGetString(doc, node->xmlChildrenNode, 1);
		}
	}
	return NULL;
}

/* get node by node name */
xmlNodePtr get_node_by_name(xmlDocPtr doc, xmlNodePtr parent, const xmlChar *name)
{
	xmlNodePtr node = NULL;
	for( node = parent->children; node != NULL; node = node->next )
	{
		if(xmlStrcmp(node->name, name) == 0 )
		{
			return node;
		}
	}
	return NULL;
}

