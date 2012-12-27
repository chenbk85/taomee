#!/usr/bin/python
# -*- coding: utf-8 -*-
import re
import xml.sax.handler
import os

def xml2obj(src):
	"""
	A simple function to converts XML data into native Python object.
	"""

	non_id_char = re.compile('[^_0-9a-zA-Z]')
	def _name_mangle(name):
		return non_id_char.sub('_', name)

	class DataNode(object):
		def __init__(self):
			self._attrs = {}	# XML attributes and child elements
			self.data = None	# child text data
		def __len__(self):
			# treat single element as a list of 1
			return 1
		def __getitem__(self, key):
			if isinstance(key, basestring):
				return self._attrs.get(key,None)
			else:
				return [self][key]
		def __contains__(self, name):
			return self._attrs.has_key(name)
		def __nonzero__(self):
			return bool(self._attrs or self.data)
		def __getattr__(self, name):
			if name.startswith('__'):
				# need to do this for Python special methods???
				raise AttributeError(name)
			return self._attrs.get(name,None)
		def _add_xml_attr(self, name, value):
			if name in self._attrs:
				# multiple attribute of the same name are represented by a list
				children = self._attrs[name]
				if not isinstance(children, list):
					children = [children]
					self._attrs[name] = children
				children.append(value)
			else:
				self._attrs[name] = value
		def __str__(self):
			return self.data or ''
		def __repr__(self):
			items = sorted(self._attrs.items())
			if self.data:
				items.append(('data', self.data))
			return u'{%s}' % ', '.join([u'%s:%s' % (k,repr(v)) for k,v in items])

	class TreeBuilder(xml.sax.handler.ContentHandler):
		def __init__(self):
			self.stack = []
			self.root = DataNode()
			self.current = self.root
			self.text_parts = []
		def startElement(self, name, attrs):
			self.stack.append((self.current, self.text_parts))
			self.current = DataNode()
			self.text_parts = []
			# xml attributes --> python attributes
			for k, v in attrs.items():
				self.current._add_xml_attr(_name_mangle(k), v)
		def endElement(self, name):
			text = ''.join(self.text_parts).strip()
			if text:
				self.current.data = text
			if self.current._attrs:
				obj = self.current
			else:
				# a text only node is simply represented by the string
				obj = text or ''
			self.current, self.text_parts = self.stack.pop()
			self.current._add_xml_attr(_name_mangle(name), obj)
		def characters(self, content):
			self.text_parts.append(content)

	builder = TreeBuilder()
	if isinstance(src,basestring):
		xml.sax.parseString(src, builder)
	else:
		xml.sax.parse(src, builder)
	return builder.root._attrs.values()[0]

#-----XML-END------------------------------------

#系统已知类型定义
g_type_conf={
	"uint32":"uint32_t",
	"uint16":"uint16_t",
	"uint8":"uint8_t",
	"int8":"int8_t",
	"uint64":"uint64_t",
	"int64":"int64_t",
	"int32":"int32_t",
	"int16":"int16_t",
	"char":"char",
	"double":"double",
};

#得到可用插件列表
def get_plugin_list(plugin_file_name_list ):
	import sys
	plugin_list=[]
	for file_name in plugin_file_name_list.split(","):
		plugin_item={}
		file_name=file_name.strip();
		dirname=os.path.dirname(file_name );
		sys.path.append(dirname);
		basename=os.path.basename(file_name);
		plugin_name=basename.split(".")[0];

		exec ("import %s"%(plugin_name) );
		exec ("plugin_item['init']=%s.init"%(plugin_name) );
		exec ("plugin_item['deal']=%s.deal"%(plugin_name) );
		exec ("plugin_item['fini']=%s.fini"%(plugin_name) );
		plugin_list.append(plugin_item);
	return plugin_list;

