import sys

def mk_includes(plugins):
	str = ""
	for x in plugins:
		str += "#include <renderer_" + x + ".hxx>\n"
	return str

	
def mk_cases(plugins):
	str = ""
	for x in plugins:
		str += "\tif(p_str == \"" + x + "\")\n\t{\n\t\treturn new renderer_" + x + "();\n\t}\n"		
	return str
	
	
def mk_body():
	str = """
#include <string>
#include <renderer_base.hxx>
<%%INCLUDES%%>

namespace ascii{

static inline renderer_base* create_instance(const ::std::string& p_str)
{
<%%CASES%%>

	return nullptr;
}

}
"""
	return str
	
	
	
def main(argv):

	plugins = argv[1:]
	
	src = mk_body()
	src = src.replace("<%%INCLUDES%%>", mk_includes(plugins))
	src = src.replace("<%%CASES%%>", mk_cases(plugins))
	
	print src
	sys.exit()
	
	
if __name__ == "__main__":
	main(sys.argv)