
# get up to date
git fetch

# get on the right branch
git checkout classforcomp
git branch

# delete previous versions
echo "delete"
rm -rf CMakeLists.txt riss coprocessor proofcheck cmake classifier libpca armadillo

# copy things here
echo "copy from source"
cp -R ../../../../CMakeLists.txt ../../../../riss ../../../../coprocessor ../../../../proofcheck ../../../../cmake ../../../../classifier ../../../../libpca ../../../../armadillo .
