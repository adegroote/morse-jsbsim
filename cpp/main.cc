#include <jsbsim_node.hh>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/program_options.hpp>

namespace pt = boost::property_tree;
namespace po = boost::program_options;

void
usage(const po::options_description& desc)
{
	std::cout << "Usage: jsbsim_node [options]\n";
	std::cout << desc;
}

int main(int argc, char **argv)
{
	po::options_description desc("Allowed options");
	desc.add_options()
		("help,h", "produce help message")
		("config,c", po::value<std::string>(), "configuration file location")
		("robot,r", po::value<std::string>(), "robot name")
		;

	po::variables_map vm;
	po::store(po::command_line_parser(argc, argv).
			options(desc).run(), vm);
	po::notify(vm);

	if (vm.count("help")) {
		usage(desc);
		return 0;
	}


	if (vm.count("config") == 0 or vm.count("robot") == 0) {
		std::cerr << "config and robot parameter are mandatories" << std::endl;
		usage(desc);
		return -1;
	}


	std::string config_file = vm["config"].as<std::string>();
	std::string robot = vm["robot"].as<std::string>();

	pt::ptree tree;
	pt::read_json(config_file, tree);

	std::string federation = tree.get<std::string>("hla.federation");

	jsbsim_node node(tree, robot);
	node.join(federation, "aircraft.fed");
	node.pause();
	node.init_fdm();
	node.publishAndSubscribe();
	node.declare();
	node.setTimeRegulation(true, true);
	node.synchronize(0);
	while (1)
		node.step();
}
