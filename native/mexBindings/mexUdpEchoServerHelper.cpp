/*
 * Copyright (C) Vamsi.  2017-18 All rights reserved.
 *
 * This copyrighted material is made available to anyone wishing to use,
 * modify, copy, or redistribute it subject to the terms and conditions
 * of the GNU General Public License version 2.
 */

#include "mex.h"
#include "mexNs3State.h"

using namespace ns3;
static int MEXIsLoaded = 0;

void cleanup(void)
{
    MEXIsLoaded = 0;
}

void* getElem(MemMngmt *obj)
{
    return obj->vptr;
}

typedef enum methodType
{
    SETATTRIBUTE,
    UDPECHOSERVERHELPER,
    INSTALL,
    DELETE,
    TOTAL_METHODS
}methodType_t;

static char methodNames[TOTAL_METHODS][CMD_BUFFER_SIZE] = {"SetAttribute", "new", "Install", "delete"};

methodType_t identifyMethod(const char* cmd)
{
    for (int i = 0; i < TOTAL_METHODS; i++)
    {
        if (!strcmp(methodNames[i], cmd))
            return methodType_t(i);
    }
    return TOTAL_METHODS;
}

void mexFunction(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
    if (!MEXIsLoaded)
    {
        mexAtExit(cleanup);
        MEXIsLoaded = 1;
    }
    uint16_t indx = 0;
    char cmd[CMD_BUFFER_SIZE];
    if (nrhs < 1 || mxGetString(prhs[INPUT_Command_String_Index], cmd, sizeof(cmd)))
    {
        mexErrMsgTxt("Check your inputs");
    }
    switch(identifyMethod(cmd))
    {
        case SETATTRIBUTE:
        {
            if ((nlhs != 0) || (nrhs > 4))
            {
                mexErrMsgTxt("enter correct number of input and output args");
            }
            std::string arg2 = std::string(mxArrayToString(prhs[2]));
            MemMngmt *tst3=  bindMngrHasListElem ((MemMngmt *)(*((uint64_t *)mxGetData(prhs[3]))));
            AttributeValue const & arg3 = *(reinterpret_cast<AttributeValue*>(tst3->vptr));
            void *obj =  getElem(bindMngrHasListElem ((MemMngmt *)(*((uint64_t *)mxGetData(prhs[1])))));
            (reinterpret_cast<UdpEchoServerHelper*>(obj))->SetAttribute(arg2, arg3);
            break;
        }
        case UDPECHOSERVERHELPER:
        {
            if ((nlhs != 1) || (nrhs > 2))
            {
                mexErrMsgTxt("enter correct number of input and output args");
            }
            LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);
            uint16_t arg2 = (uint16_t) *mxGetPr(prhs[1]);
            UdpEchoServerHelper *obj = new UdpEchoServerHelper(arg2);
            MemMngmt *tst = bindMngrNewElem();
            tst->vptr = reinterpret_cast<void*>(obj);
            tst->objType = UDPECHOSERVERHELPOBJ;
            plhs[0] = mxCreateNumericMatrix(1, 1, mxUINT64_CLASS, mxREAL);
            *((uint64_t *)mxGetData(plhs[0])) = reinterpret_cast<uint64_t>((MemMngmt*)tst);
            break;
        }
        case INSTALL:
        {
            if ((nlhs != 1) || (nrhs > 3))
            {
                mexErrMsgTxt("enter correct number of input and output args");
            }
            switch(nrhs)
            {
                case 3:
                {
                    MemMngmt *tst2=  bindMngrHasListElem ((MemMngmt *)(*((uint64_t *)mxGetData(prhs[2]))));
                    void *obj =  getElem(bindMngrHasListElem ((MemMngmt *)(*((uint64_t *)mxGetData(prhs[1])))));
#ifndef MANUAL_CHANGE
                    Ptr<ns3::Node> arg2 = *(reinterpret_cast<Ptr<ns3::Node>*>(tst2->vptr));
                    ApplicationContainer *tmp = new ApplicationContainer((reinterpret_cast<UdpEchoServerHelper*>(obj))->Install(arg2));
#else
                    ApplicationContainer *tmp;
                    if (tst2->objType == NODEOBJ_SP)
                    {
                        Ptr<Node> arg2 = *(reinterpret_cast<Ptr<Node>*>(tst2->vptr));
                        tmp = new ApplicationContainer((reinterpret_cast<UdpEchoServerHelper*>(obj))->Install(arg2));
                    }
                    else if (tst2->objType == NODECONTOBJ)
                    {
                        NodeContainer arg2 = *(reinterpret_cast<NodeContainer*>(tst2->vptr));
                        tmp = new ApplicationContainer((reinterpret_cast<UdpEchoServerHelper*>(obj))->Install(arg2));
                    }
                    else
                        mexErrMsgTxt("error in udp server install");
#endif
                    MemMngmt *tst = bindMngrNewElem();
                    tst->vptr = reinterpret_cast<void*>(tmp);
                    tst->objType = APPCONTOBJ;
                    plhs[0] = mxCreateNumericMatrix(1, 1, mxUINT64_CLASS, mxREAL);
                    *((uint64_t *)mxGetData(plhs[0])) = reinterpret_cast<uint64_t>((MemMngmt*)tst);
                    break;
                }
            }
            break;
        }
        case DELETE:
        {
            MemMngmt *obj =  bindMngrHasListElem ((MemMngmt *)(*((uint64_t *)mxGetData(prhs[1]))));
            if (obj)
            {
                if (obj->objType == UDPECHOSERVERHELPOBJ)
                    delete reinterpret_cast<UdpEchoServerHelper*>(obj->vptr);
                bindMngrDelSingleElem(obj);
            }
            break;
        }
        default:
            break;
    }
}
