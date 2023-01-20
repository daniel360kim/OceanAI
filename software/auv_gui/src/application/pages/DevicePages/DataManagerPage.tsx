import {
    ChartContainer,
    LineChart,
    RealTimeDomain,
    TimeAxis,
    VerticalAxis,
    ZoomBrush
  } from '@electricui/components-desktop-charts'
  
  import { Card, Colors } from '@blueprintjs/core'
  import { Composition } from 'atomic-layout'
  import { IntervalRequester } from '@electricui/components-core'
  import React from 'react'
  import { RouteComponentProps } from '@reach/router'
  import { HTMLTable } from '@blueprintjs/core'
  import { Callout } from '@blueprintjs/core'
  import { Statistic, Statistics } from '@electricui/components-desktop-blueprint'
  import { StateIndicator } from 'src/application/components/StateIndication'
  import { MessageDataSource } from '@electricui/core-timeseries'
  import { NumberInput } from '@electricui/components-desktop-blueprint'
  import { Printer } from '@electricui/components-desktop'
  
  const layoutDescription = `
    Title Info
    LoopChart TempChart
  `
  

  
  const baseSpeed = 100
  
  export const DataManagerPage = (props: RouteComponentProps) => {
    return (
      <React.Fragment>
  
        <Composition areas={layoutDescription} gap={10} autoCols="1fr">
          {Areas => (
            <React.Fragment>


                <NumberInput
                      accessor="fc"
                      intent="warning"
                      min={0}
                      max={1}
                      leftIcon="dashboard"
                      large
                    />
                <Printer accessor="fc" />


               
            </React.Fragment>
          )}
        </Composition>
      </React.Fragment>
    )
  }
  