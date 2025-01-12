from setuptools import setup

try:
    from wheel.bdist_wheel import bdist_wheel as _bdist_wheel
    class bdist_wheel(_bdist_wheel):
        def finalize_options(self):
            _bdist_wheel.finalize_options(self)
            # Mark us as not a pure python package
            self.root_is_pure = False

        # def get_tag(self):
        #     python, abi, plat = _bdist_wheel.get_tag(self)
        #     # We don't contain any python source
        #     python, abi = 'py3', 'none'
        #     return python, abi, plat
except ImportError:
    bdist_wheel = None

setup(
    # ...
    cmdclass={'bdist_wheel': bdist_wheel},
)