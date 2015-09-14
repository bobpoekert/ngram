from tornado.httpclient import AsyncHTTPClient
import keys
import urllib
import lxml.etree as etree

def query_url(query, **props):
    props['query'] = query
    return '%s&%s' % (keys.yandex_base_url, urllib.urlencode(props))

def extract_yandex_xml(tree):
    results = tree.xpath('//results')[0]
    count_tags = results.xpath('//found')
    counts = {}
    for tag in count_tags:
        try:
            counts[tag.get('priority')] = int(tag.text)
        except:
            pass
    docs = []
    for doc in results.xpath('//doc'):
        row = {}
        row['url'] = doc.xpath('//url/text()')[0]
        row['domain'] = doc.xpath('//domain/text()')[0]
        row['title'] = ' '.join(doc.xpath('//title/*/text()'))
        row['passages'] = doc.xpath('//paspages/passage/*/text()')
        row['cache_url'] = doc.xpath('//saved-copy-url/text()')[0]
        row['size'] = int(doc.xpath('//size/text()')[0])

        props_dict = {}
        props = doc.xpath('//properties/*')
        for prop in props:
            props_dict[prop.tag] = prop.text
        row['props'] = props_dict
        docs.append(row)

    return docs


class Search(object):

    def __init__(self, query, callback, **props):
        self.callback = callback
        self.query = query
        self.props = props
        self.req = AsyncHTTPClient()
        self.req.fetch(query_url(query, **props), callback=self.on_response)

    def on_response(self, response):
        payload = response.body
        tree = etree.fromstring(payload)
        self.callback(extract_yandex_xml(tree))

