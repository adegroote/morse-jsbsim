#include <jsbsim_node.hh>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace pt = boost::property_tree;

int main(int argc, char **argv)
{
	pt::ptree tree;
	pt::read_json("/tmp/jsbsim_config.json", tree);

	std::string federation = tree.get<std::string>("hla.federation");

	jsbsim_node node("jsbsim");
	node.join(federation, "aircraft.fed");
	node.pause();
	node.init_fdm("simple_quad");
	node.publishAndSubscribe();
	node.declare();
	node.setTimeRegulation(true, true);
	node.synchronize(0);
	while (1)
		node.step();
}
