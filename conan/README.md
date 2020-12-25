# About

example conan profiles

## Usage

```bash
conan create . conan/stable --profile clang.profile
conan install conanfile.py --profile clang.profile

# clean build cache
conan remove "*" --build --force
```
