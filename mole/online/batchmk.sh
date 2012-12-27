make clean all
cd games
cd football
make clean all
cd ../seesaw
make clean all
cd ../mimic_show
make clean all
cd ../cheer_squad_show
make clean all
cd ../little_game_pos
make clean all
cd ../..
rm ./games/lib*.so
cp -v ./games/*/lib*.so ./games/*/*.xml ./games/
rm ./log/*
./startup.sh
