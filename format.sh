#/bin/sh

FORMATER=$(eval "clang-format-9 --version | egrep '\w*(9\.0\.0)'")
if [ -z "$FORMATER" ];
then
    echo "clang-format-9.0.0 not found"
    echo "please, install format-tool from terminal 'sudo apt install clang-format-9'";
    exit 1;
else
    echo "clang-format-9.0.0 found, lets format everything";
fi

files=$(find . \
                -regex '.*\.\(cpp\|c\|hpp\|h\)'
)
for file in $files
do
   clang-format-9 -i -style=file $file
done

exit 0;
