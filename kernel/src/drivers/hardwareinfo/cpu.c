int cpuSpeed() {
    int sum = 0;
    for(int i = 0;i<100;i++) {
        int stsc=getRTCtime();
        for (int i=0x1000;i>0;i--);
        int etsc=getRTCtime();
        int difa = etsc-stsc/0x1000;
        sum+=difa;
    }
    return sum/100;
}