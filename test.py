from whoosh.index import create_in
from whoosh.qparser import QueryParser
from whoosh.fields import *

schema = Schema(title = TEXT(stored = True), path = ID(stored = True), content = TEXT)
ix = create_in('./', schema)
writer = ix.writer()
writer.add_document(title = u'First document', path = u'/a', content = u'This is the first document')
writer.add_document(title = u'Second document', path = u'/a', content = u'This is the second document')
writer.commit()

with ix.searcher() as searcher:
    query = QueryParser('content', ix.schema).parse('first')
    results = searcher.search(query)
    print(results[0])