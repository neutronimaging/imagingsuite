#include <iostream>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <sstream>
#include <stdexcept>
#include "output.hpp"
#include "xml_util.hpp"

using std::cout;
using std::endl;
using std::ostringstream;
using std::runtime_error;
using std::string;
using std::vector;

namespace nxsum {
  void print(const Item &item, const string &value, const Config &config){
    if (config.show_label)
      {
        cout << item.label << ':';
      }
    cout << value << endl;
  }

  void printError(const Item &item, const string &msg, const Config &config) {
    if(!config.verbose)
      {
        return;
      }
    cout << '[' << item.label << ',' << item.path;
    if (!item.operation.empty())
      {
        cout << ',' << item.operation;
      }
    cout << "] ERROR: " << msg << endl;
  }

  static void printStd(const string &filename, const vector<Item> &preferences,
                       const vector<string> &values,
                       const vector<bool> &isError, const Config &config) {
    if (config.multifile)
      {
        cout << "********** " << filename << endl;
      }
    
    size_t length = preferences.size();
    for (size_t i = 0; i < length; ++i)
      {
        if (isError[i])
          {
            printError(preferences[i], values[i], config);
          }
        else
          {
            print(preferences[i], values[i], config);
          }
      }
  }

#if defined(LIBXML_TREE_ENABLED) && defined(LIBXML_OUTPUT_ENABLED)
  static void writeItem(xmlNodePtr &parent, const string &label, const string &path, const string &value) {
    xmlNodePtr node = NULL;
    if (value.empty())
      {
        node = xmlNewChild(parent, NULL, item_name, NULL);
      }
    else
      {
        node = xmlNewTextChild(parent, NULL, item_name, BAD_CAST value.c_str());
      }
    if (!path.empty())
      {
        xmlNewProp(node, path_name, BAD_CAST path.c_str());
      }
    if (!label.empty())
      {
        xmlNewProp(node, label_name, BAD_CAST label.c_str());
      }
  }

  static void printXml(const string &filename, const vector<Item> &preferences,
                       const vector<string> &values,
                       const vector<bool> &isError, const Config &config) {
    // set up variables for creating the document
    xmlDocPtr doc = NULL;        // document pointer
    xmlNodePtr root_node = NULL; // node pointers

    // create the document and the root node
    doc = xmlNewDoc(BAD_CAST "1.0");
    root_node = xmlNewNode(NULL, root_name);
    xmlDocSetRootElement(doc, root_node);

    // add the name of the file to the output
    xmlNewProp(root_node, BAD_CAST "filename", BAD_CAST filename.c_str());

    // loop through the values
    size_t length = preferences.size();
    for (size_t i = 0; i < length; ++i)
      {
        if (!isError[i]) {
          writeItem(root_node, preferences[i].label, preferences[i].path, values[i]);
        }
      }

    // write out to stdout
    xmlSaveFormatFileEnc("-", doc, "UTF-8", 1);
    xmlFreeDoc(doc);
    xmlCleanupParser();
  }
#endif

  void print(const string &filename, const vector<Item> &preferences,
             const vector<string> &values, const vector<bool> &isError,
             const Config &config) {
    if (preferences.size() != values.size())
      {
        ostringstream buffer;
        buffer << "Number of items (" << preferences.size()
               << ") does not match number of values (" << values.size()
               << ')';
        throw runtime_error(buffer.str());
      }
    if (preferences.size() != isError.size())
      {
        ostringstream buffer;
        buffer << "Number of items (" << preferences.size()
               << ") does not match number of \"isError\" (" << isError.size()
               << ')';
        throw runtime_error(buffer.str());
      }

    if (config.print_xml)
      {
#if defined(LIBXML_TREE_ENABLED) && defined(LIBXML_OUTPUT_ENABLED)

        printXml(filename, preferences, values, isError, config);
#else
        cout << "XML OUTPUT NOT COMPILED IN" << endl;
#endif
      }
    else
      {
        printStd(filename, preferences, values, isError, config);
      }
  }
}
