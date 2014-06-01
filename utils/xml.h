#include <libxml/tree.h>
#include <libxml/parser.h>
int xml_init();
int xml_close();
xmlChar* get_attr_by_name(xmlDocPtr doc, xmlNodePtr node, const xmlChar *name);

xmlChar *get_value_by_name(xmlDocPtr doc, xmlNodePtr parent, const xmlChar *name);

xmlNodePtr get_node_by_name(xmlDocPtr doc, xmlNodePtr parent, const xmlChar *name);

xmlDocPtr create_xml_doc();

xmlNodePtr create_xml_root_node( xmlDocPtr doc, char *name );

xmlNodePtr add_xml_child( xmlNodePtr node, char *name, char *value );

int add_xml_child_prop( xmlNodePtr node, char *name, char *value );
