#ifndef __DOCUMENTS_H__
#define __DOCUMENTS_H__

/* a header that provides access to several json documents broken into chunks of
 * less than 4k, cause C99 says that's what we should do and YAJL likes streams */

extern const char ** g_documents[];
int num_docs(void);
const char ** get_doc(int i);
unsigned int doc_size(int i);

#endif
