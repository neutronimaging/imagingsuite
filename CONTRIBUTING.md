Your contribution to this project is very welcome. It is however important to stay within some basic house rules.

# Branches and issues
1. Create an issue for each contribution (bug, new code, improvements, etc). It is beneficial to make check lists in the issue. It makes it possible to track progress.
2. All new development shall be in branches
3. The branches are created based on an issue and shall be named issue###_shortname.
4. Merging into master is only done by @anderskaestner. Your steps prepare a merge:
    1. Merge master into your branch.
         ```bash
             git checkout <mybranch>
             git merge master
         ```
    2. Rebuild the code. Fix issues
    3. Run unit tests. Fix failed tests
5. Merges are done using pull requests, which are reviewed by @anderskaestner before merging.
# Unit tests
1. Implement unit tests for your contributions
    1. New code
    2. Debugging; add tests in existing test classes or implement a new test class if needed.
2. Test data is provided in [test data repository](https://github.com/neutronimaging/TestData)     

# Coding style
Use the following styles in the code:
1. Braces
   ```c++
   if (a==b)
   {
     // the code
   }
   ```
2. Function names: To be written
3. Class names: To be written
4. Variables: To be written

# Open questions
This document is still under construction. Please don't hesitate to contact @anderskaestner if something is unclear
