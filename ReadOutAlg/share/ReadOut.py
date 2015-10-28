#!/usr/bin/env python
# -*- coding:utf-8 -*-
#anthor: HxWang

import Sniper

def get_parser():
    import argparse
    parser = argparse.ArgumentParser(description='Run Simulation.')
    parser.add_argument("--evtmax", type=int, default=10, help='events to be processed')
    parser.add_argument("--seed", type=int, default=5, help='seed')
    parser.add_argument("--start", default="1970-01-01 08:00:00", help='starting time')#year, month, day, h, min, s; don't chang the format
    parser.add_argument("--end", default="2014-09-01 12:05:00", help='ending time')#year, month, day, h, min, s
    parser.add_argument("--output", default="sample_mixingDetSim.root", help="output file name")
    parser.add_argument("--mode", type=int, default=1, choices=[1,2], help="mixing mode, 1:hit level, 2:readout level")
    parser.add_argument("--elec", action="store_true", help="add elecsim")
    parser.add_argument("--detoption", default="Acrylic",choices=["Acrylic", "Balloon"], help="Det Option")
    return parser


TOTALPMTS = {"Acrylic": 17746, "Balloon": 18306}


def setup_WaveSim(task):
    #data registrition
    drs = task.createSvc("DataRegistritionSvc")
    #buffer service
    bufMgr=task.createSvc("BufferMemMgr")
    bufMgr.property("TimeWindow").set([-1.0,1.0])
    #add UnpackingAlg 
    import WaveformSimAlg
    WaveAlg = task.createAlg("WaveformSimAlg") 


def setup_PreTrg(task):
    #data registrition
    drs = task.createSvc("DataRegistritionSvc")
    #buffer service
    bufMgr=task.createSvc("BufferMemMgr")
    bufMgr.property("TimeWindow").set([-1.0,1.0])
    #add UnpackingAlg 
    import PreTrgAlg
    PreTrg = task.createAlg("PreTrgAlg") 


def setup_PMTSim(task):
    #data registrition
    drs = task.createSvc("DataRegistritionSvc")
    #buffer service
    bufMgr=task.createSvc("BufferMemMgr")
    bufMgr.property("TimeWindow").set([-1.0,1.0])
    #add UnpackingAlg 
    import PMTSimAlg
    PMTSim = task.createAlg("PMTSimAlg") 


def setup_unpacking(task):
    #data registrition
    drs = task.createSvc("DataRegistritionSvc")
    #buffer service
    bufMgr=task.createSvc("BufferMemMgr")
    bufMgr.property("TimeWindow").set([-1.0,1.0])
    #add UnpackingAlg 
    import UnpackingAlg
    Unpack = task.createAlg("UnpackingAlg") 

def setup_EvtMixing(task):
    #data registrition
    drs = task.createSvc("DataRegistritionSvc")
    #buffer service
    bufMgr=task.createSvc("BufferMemMgr")
    bufMgr.property("TimeWindow").set([-1.0,1.0])
    #add EvtMixingAlg
    import EvtMixingAlg
    mixAlg=task.createAlg("EvtMixingAlg")
    mixAlg.property("RateMap").set(rateMap)
    mixAlg.property("TaskMap").set(taskMap)
    #mixAlg.property("SimTime").set(simTime) 
    #mixAlg.property("BufSize").set(bufSize)
    #mixAlg.property("Mode").set(args.mode)
    #mixAlg.property("seed").set(args.seed)






if __name__ == "__main__":

    import DataRegistritionSvc
    import BufferMemMgr
    import RootIOSvc

    Sniper.setLogLevel(3)
    #Sniper.setLogLevel(6)
    parser = get_parser()
    args = parser.parse_args()

    samples = [ 
            {"name": "IBD_sample_1",
                "rate": 10000, #Hz 
                "input": ["IBD_1.root", "IBD_2.root"]},
            {"name": "IBD_sample_2",
                "rate": 20000, #Hz 
                "input": ["IBD_3.root", "IBD_4.root"]},
            {"name": "IBD_sample_3",
                "rate": 30000, #Hz
                "input": ["IBD_5.root", "IBD_6.root"]}
            ]

    #    samples = [ 
#            {"name": "sample_1",
#                "rate": 1000, #Hz 
#                "input": ["sample_detsim.root"]},
#            ]

    bufSize = 5.0 #second

    sampleNum = len(samples)
    taskList=[]
    inputList=[]
    rateMap={}
    taskMap={}
    for i in range(sampleNum):
        key_sample_name = (samples[i]["name"])
        rate = (samples[i]["rate"])
        inputList.append(samples[i]["input"])
        taskList.append(samples[i]["name"])
        rateMap[key_sample_name]=rate
        taskMap[key_sample_name]=taskList[i]


    #create top task
    task_top = Sniper.Task("task_top")
    task_top.asTop()
    task_top.setEvtMax(args.evtmax)
    #task_top.setLogLevel(0)






    #add RootRandomSvc
    import RootRandomSvc
    task_top.property("svcs").append("RootRandomSvc")
    rndm = task_top.find("RootRandomSvc")
    rndm.property("Seed").set(args.seed)

    #add GlobalTimeSvc
    import GlobalTimeSvc
    task_top.property("svcs").append("GlobalTimeSvc")
    time = task_top.find("GlobalTimeSvc")
    time.property("start").set(args.start)


    #add ElecBufferMgrSvc
    import ElecBufferMgrSvc
    task_top.property("svcs").append("ElecBufferMgrSvc")
    bufferMgr = task_top.find("ElecBufferMgrSvc")


    #add PmtParamSvc
    import PmtParamSvc
    task_top.property("svcs").append("PmtParamSvc")
    bufferMgr = task_top.find("PmtParamSvc")





    #add ReadOutAlg
    import ReadOutAlg
    readoutAlg = task_top.createAlg("ReadOutAlg")




    #add WaveformSimTask
    sub_task_WaveSim = task_top.createTask("Task/WaveformSimTask")
    setup_WaveSim(sub_task_WaveSim)


    #add PreTrgTask
    sub_task_PreTrg = task_top.createTask("Task/PreTrgTask")
    setup_PreTrg(sub_task_PreTrg)


    #add PMTSimTask
    sub_task_PMTSim = task_top.createTask("Task/PMTSimTask")
    setup_PMTSim(sub_task_PMTSim)


    #add unpacking task
    sub_task_unpack = task_top.createTask("Task/UnpackingTask")
    setup_unpacking(sub_task_unpack)


    #add EvtMix tasks
    sub_task_EvtMix = task_top.createTask("Task/EvtMixingTask")
    setup_EvtMixing(sub_task_EvtMix)




    #add input task
    subTask={}
    readin={}
    for i in range(sampleNum):
        subTask[i] = task_top.createTask("Task/"+taskList[i])    #taskList[2]: sample2
        #subTask[i].setLogLevel(0)
        subTask[i].createSvc("DataRegistritionSvc")
        readin[i]=subTask[i].createSvc("RootInputSvc/InputSvc")
        readin[i].property("InputFile").set(inputList[i])
        bufMgrTmp=subTask[i].createSvc("BufferMemMgr")
        bufMgrTmp.property("TimeWindow").set([-1.0,1.0])



    #run    
    task_top.show()
    task_top.run()
