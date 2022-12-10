/// @cake.cc g++ -std=c++20 -fno-rtti -Wall -Wextra -Wpedantic -Werror=format -Werror=return-type -Wno-stringop-overflow -Wno-comment \
///          -march=x86-64 -O2 -flto=auto -s -o target/cakefile cakefile.cpp -l:dimgel-cake1.a -l:dimgel-util1.a -lxxhash

#include <dimgel-cake1/aux.h>

int main(int argc, char** argv) {
	return with(argc, argv, [](Oven& v) {
		// Now called by aux::with().
//		v.setDefaultTarget("all");

		// Don't bother creating rules we won't need.
		if (v.getActualTargetName() == "clean") {

			// Simple one-step tasks don't need rule machinery at all, even though Oven.cook() disables ThreadPool for single-rule graph.
//			clean(v);
			// For this demo, I'd rather keep compiled target/cakefile.
			cleanExceptCakefile(v);
			return;
		}

		DirMaker dm(v);

		// S = std::string, VS = std::vector<S>. Command and each arg are in separate strings, for execvp(3).
		VS ccll {"g++", "-fno-rtti", "-march=x86-64", "-O2", "-flto=auto", "-s"};
		VS cc = ccll + VS{"-MMD", "-MP", "-c"};
		VS& ll = ccll;

		// T = TargetRef (variant: string or "resolved" pair(ruleImpl*, targetIndex)), UT = std::unordered_set<T>.
		// We can save cake some work by passing around "resolved" dependencies instead of strings.
		UT objs;

		findFilesByNameSuffix("src", {".cpp"}, [&](S cpp) {

			// No patterns like `%.o: %.cpp`, no automatic vars. Rule for each .cpp file is added manually -- stupid and straightforward.
			// On the other hand, it's not THAT much more verbose than `make`, and with C++ you can write any helper functions you like.
			S o = changePrefixAndExt("src/", "target/build/", cpp, "o");
			S d = changeExt(o, "d");

//			objs += v.rule(o, {cpp, {d, PhonyIfMissing}}, {
//			objs += v.rule(o, cpp + parseDFile(v, {.d = d, .o = o}), {
			objs += v.rule(o, parseDFile(v, {.d = d, .o = o}), {
				// Internal command is {function<void()>}: we don't waste time on fork/exec for what we can do ourselves.
				// ATTENTION! Capturing `o` by value because it goes out of scope at the end of for() iteration, before cook() is run!
				{[&v, &dm, o] {
					if (v.getParams().verbosity >= Verbosity_Default) {
						v.getLog().compile("%s", o.c_str());
					}
					dm.mkdirRecursive(getParentDir(o));
				}},
				// External command is SS.
				cc + VS{"-o", o, cpp}
			});
		});

//		v.setHashCacheEnabledByDefault(false);

		S binName = "target/hello";
		auto bin = v.rule({binName, NoHash}, objs, {
			{[&v, &binName] {
				if (v.getParams().verbosity >= Verbosity_Default) {
					v.getLog().link("%s", binName.c_str());
				}
			}},
			ll + VS{"-o", binName} + toNames(objs)
		});

		v.phony("all", {bin});

		// Let's go.
		v.cook();
	});
}
