#include <jsbsim_node.hh>

int main(int argc, char **argv)
{
	jsbsim_node node("jsbsim");
	node.join("morse_fdm", "aircraft.fed");
	node.pause();
	node.init_fdm("simple_quad");
	node.publishAndSubscribe();
	node.declare();
	node.setTimeRegulation(true, true);
	node.synchronize(0);
	while (1)
		node.step();
}
