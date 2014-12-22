#!/usr/bin/env python
# -*- coding:utf-8 -*-
#anthor: HxWang

import Sniper

def get_parser():
    import argparse
    parser = argparse.ArgumentParser(description='Run Simulation.')
    parser.add_argument("--evtmax", type=int, default=-1, help='events to be processed')
    parser.add_argument("--seed", type=int, default=0, help='seed')
    parser.add_argument("--start", default="2014-09-01 12:00:00", help='starting time')#year, month, day, h, min, s; don't chang the format
    parser.add_argument("--end", default="2014-09-01 12:05:00", help='ending time')#year, month, day, h, min, s
    parser.add_argument("--output", default="sample_mixingDetSim.root", help="output file name")
    parser.add_argument("--mode", type=int, default=1, choices=[1,2], help="mixing mode, 1:hit level, 2:readout level")
    parser.add_argument("--elec", action="store_true", help="add elecsim")
    parser.add_argument("--detoption", default="Acrylic",choices=["Acrylic", "Balloon"], help="Det Option")
    return parser


TOTALPMTS = {"Acrylic": 17746, "Balloon": 18306}


if __name__ == "__main__":
    parser = get_parser()
    args = parser.parse_args()

    samples = [ 
                {"name": "sample1",
                 "rate": 1.0, #Hz 
                 "input": ["gamma_1MeV_1.root", "gamma_1MeV_2.root"]},
                {"name": "sample2",
                 "rate": 2.0, #Hz 
                 "input": ["gamma_1MeV_3.root", "gamma_1MeV_4.root"]},
                {"name": "sample3",
                 "rate": 3.0, #Hz
                 "input": ["gamma_1MeV_5.root", "gamma_1MeV_6.root"]}
    ]

    bufSize = 5.0 #second
    
    sampleNum = len(samples)
    sampleList=[]
    rateList=[]
    taskList=[]
    inputList=[]
    rateMap={}
    taskMap={}
    for i in range(sampleNum):
        sampleList.append(samples[i]["name"])
        rateList.append(samples[i]["rate"])
        inputList.append(samples[i]["input"])
        taskList.append(samples[i]["name"])
        rateMap[sampleList[i]]=rateList[i]
        taskMap[sampleList[i]]=taskList[i]

    simTime = [args.start,args.end]

    #create top task
    task_top = Sniper.Task("task_top")
    task_top.asTop()
    task_top.setEvtMax(args.evtmax)
    task_top.setLogLevel(5)
    
    #add DataRegistritionSvc to output data
    import DataRegistritionSvc
    drs = task_top.createSvc("DataRegistritionSvc")
    #drs.property("EventToPath").set({"JM::SimEvent":"/Event/SimEvent"})
    
    import BufferMemMgr
    bufMgr=task_top.createSvc("BufferMemMgr")
    bufMgr.property("TimeWindow").set([-1.0,1.0])
    
    import RootIOSvc
    readout = task_top.createSvc("RootOutputSvc/OutputSvc")
    if args.elec:
        readout.property("OutputStreams").set({"/Event/ElecEvent":args.output})
    else:
        readout.property("OutputStreams").set({"/Event/SimEvent":args.output})
    
    
    #add sub tasks
    subTask={}
    readin={}
    for i in range(sampleNum):
        subTask[i] = task_top.createTask("Task/"+taskList[i])
        subTask[i].setLogLevel(3)
        subTask[i].createSvc("DataRegistritionSvc")
        readin[i]=subTask[i].createSvc("RootInputSvc/InputSvc")
        readin[i].property("InputFile").set(inputList[i])
        bufMgrTmp=subTask[i].createSvc("BufferMemMgr")
        bufMgrTmp.property("TimeWindow").set([-1.0,1.0])

    
    #add alg
    import Mixing
    mixAlg=task_top.createAlg("MixingAlg/mixAlg")
    mixAlg.property("SimTime").set(simTime) 
    mixAlg.property("BufSize").set(bufSize)
    mixAlg.property("RateMap").set(rateMap)
    mixAlg.property("TaskMap").set(taskMap)
    mixAlg.property("Mode").set(args.mode)
    mixAlg.property("seed").set(args.seed)

   
    # == Elec Sim ==
    if args.elec:
        import os
        elecroot = os.environ["ELECSIMROOT"]
        pmt_data = os.path.join(elecroot, "share", "PmtData.root")
        total_pmt = TOTALPMTS[args.detoption]
        Sniper.loadDll("libElecSim.so")
        elecsim = task_top.createAlg("ElecSimAlg")
        elecsim.property("PmtTotal").set(total_pmt)
        elecsim.property("PmtDataFile").set(pmt_data)
        #elecsim.property("split_evt_time").set(10000)
        #elecsim.property("simFrequency").set(1e9)
        #elecsim.property("noiseAmp").set(3.5e-3)
        #elecsim.property("speAmp").set(0.010)
        #elecsim.property("preTimeTolerance").set(300)
        #elecsim.property("postTimeTolerance").set(2000)
        elecsim.property("UseCurrentBuffer").set(True)
            
    
    #run    
    task_top.show()
    task_top.run()
    
    
