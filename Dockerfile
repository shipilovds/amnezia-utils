FROM vookimedlo/ubuntu-qt:qt6_latestdistroofficial_gcc_jammy
COPY . /code
WORKDIR code
RUN qmake6 main.pro
RUN make clean
RUN make
