.PHONY: cloc help

cloc: ## Count line of code
	@./ci/make_scripts/cloc.sh

dep: ## Show cpp dependencies
	@cpp-dependencies --graph dependencies.dot .

conanfile: ## Update conan file in all places
	@cp -f conanfile.txt ./ci/docker/builder_ubuntu_20_04_clang_10/conanfile.txt
	@cp -f conanfile.txt ./ci/docker/builder_ubuntu_20_04_gcc_9/conanfile.txt

help: ## Display this help screen
	@grep -E '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}'
