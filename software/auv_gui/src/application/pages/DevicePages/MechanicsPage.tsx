import React from 'react'
import { RouteComponentProps } from '@reach/router'
import { Card, Colors } from '@blueprintjs/core'
import { ProgressBar } from '@electricui/components-desktop-blueprint'
import { MessageDataSource } from '@electricui/core-timeseries'
import { IntervalRequester } from '@electricui/components-core'
import { Composition } from 'atomic-layout'
import { DataSourcePrinter } from '@electricui/components-desktop-charts'
import { Printer } from '@electricui/components-desktop'
import { StateIndicator } from 'src/application/components/StateIndication'
import { Statistic, Statistics } from '@electricui/components-desktop-blueprint'

const currentPositionDS = new MessageDataSource('step_pos')
const targetPositionDS = new MessageDataSource('step_tar')
const stepSpeedDS = new MessageDataSource('step_speed')
const stepAccelDS = new MessageDataSource('step_accel')


const navigationLayoutDescription = `
    Controls Charts
`
const stepPosDS = new MessageDataSource('step_pos')

export const MechanicsPage = (props: RouteComponentProps) => {
  return (
    <React.Fragment>


      <Composition areas={navigationLayoutDescription} gap={10} autoCols="1fr">
        {Areas => 
        <React.Fragment>
          <Areas.Controls>
            <Card>
              <ProgressBar
                accessor="step_pos"
                min={0}
                max={27000}
                intent="success"
                stripes/>
              

                <h5>&emsp; &ensp; 0 &ensp; &nbsp; 5 &ensp; &nbsp; 10 &ensp; &nbsp; 15 &ensp; &nbsp; 20 &ensp; &nbsp; 25 &ensp; &nbsp; 30 &ensp; &nbsp; 35 &ensp; &nbsp; 40 &ensp; &nbsp; 45 &ensp; &nbsp; 50 &ensp; &nbsp; 55 &ensp; &nbsp; 60 &ensp; &nbsp; 65 &ensp; &nbsp; 70 &ensp; &nbsp; 75 &ensp; &nbsp; 80 &ensp; &nbsp; 85 &ensp; &nbsp; 90 &ensp; &nbsp; 95 &ensp; &nbsp; 100</h5>
                <br></br>

                <Statistics>
                  <Statistic
                    label="Current Position (half steps)"
                    accessor="step_pos"
                    color={Colors.BLUE5}
                    />
                    <Statistic
                    label="Target Position (half steps)"
                    accessor="step_tar"
                    color={Colors.RED5}
                    />
                </Statistics>
                <br></br>
                <Statistics>
                  <Statistic
                    label="Current Speed"
                    accessor="step_speed"
                    color={Colors.GREEN5}
                    />
                    <Statistic
                    label="Current Acceleration"
                    accessor="step_accel"
                    color={Colors.ORANGE5}
                    />
                </Statistics>

            </Card>
            <StateIndicator />
          </Areas.Controls>

        </React.Fragment>}
      </Composition>
    </React.Fragment>
  )
}
