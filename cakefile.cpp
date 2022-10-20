/// @cake.cc g++ -std=c++20 -fno-rtti -Wall -Wextra -Wpedantic -Werror=format -Werror=return-type -Wno-comment \
///          -march=x86-64 -O2 -flto=auto -s -o target/cakefile cakefile.cpp -l:libdimgel-cake-1.a -l:libdimgel-util-1.a -lxxhash
// Optional. Technically. You'll surely want to adjust compiler options and link more libs.

#include <dimgel-cake-1/aux.h>

int main(int argc, char** argv) {
	with(argc, argv, [] (Oven& v, aux::Log& log) {

		DirMaker dirMaker(v);

		v.setDefaultTarget("all");

		// Don't bother creating rules we won't need.
		if (v.getActualTargetName() == "clean") {

			// Adds phony rule "clean" that removes "target" directory.
//			pClean(v);
			// For this demo, I have to keep compiled target/cakefile.
			pCleanExceptCakefile(v);

		} else {

			// S = std::string, SS = std::vector<S>. Command and each arg are in separate strings, for execvp(3).
			SS ccll {"g++", "-fno-rtti", "-march=x86-64", "-O2", "-flto=auto", "-s"};
			SS cc = ccll + SS{"-MMD", "-MP", "-c"};
			SS& ll = ccll;

			// T = TargetRef (variant: string or "resolved" pair(ruleImpl*, targetIndex)), TT = std::unordered_set<T>.
			// We can save cake some work by passing around "resolved" dependencies instead of strings.
			TT objs;

			findFilesByNameSuffix("src", {".cpp"}, [&](S cpp) {

				// No patterns like `%.o: %.cpp`, no automatic vars. Rule for each .cpp file is added manually -- stupid and straightforward.
				// On the other hand, it's not THAT much more verbose than `make`, and with C++ you can write any helper functions you like.
				S o = "target/build/" + util::changeExt(util::removePrefix(cpp, "src/"), "o");
				S d = util::changeExt(o, "d");

//				objs += v.rule(o, {cpp, {d, PhonyIfMissing}}, {
//				objs += v.rule(o, cpp + parseDFile(v, {.o = o, .d = d}), {
				objs += v.rule(o, parseDFile(v, {.o = o, .d = d}), {
					// Internal command is {function<void()>}: we don't waste time on fork/exec for what we can do ourselves.
					// ATTENTION! Here I capture `o` by value because it goes out of scope at the end of for() iteration, before cook() is run!
					{[o, &dirMaker, &log] {
						log.compile("%s", o.c_str());
						dirMaker.mkdirRecursive(util::getParentDir(o));
					}},
					// External command is SS.
					cc + SS{"-o", o, cpp}
				});
			});

//			v.setHashCacheEnabledByDefault(false);

			S binName = "target/hello";
			auto bin = v.rule({binName, NoHash}, objs, {
				{[binName, &log] {
					 log.link("%s", binName.c_str());
//					 dirMaker.mkdirRecursive(util::getParentDir(binName).c_str());
				}},
				ll + SS{"-o", binName} + toNames(objs)
			});

			v.phony("all", {bin});
		}

		// Let's go.
		v.cook();
	});
}
