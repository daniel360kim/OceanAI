import {
  ChartContainer,
  LineChart,
  RealTimeDomain,
  TimeAxis,
  VerticalAxis,
  TriggerDomain,
} from '@electricui/components-desktop-charts'

import { Card, Colors } from '@blueprintjs/core'
import { Composition } from 'atomic-layout'
import { RouteComponentProps } from '@reach/router'
import { MessageDataSource } from '@electricui/core-timeseries'
import { Printer } from '@electricui/components-desktop'
import React, { useEffect, useRef, useState } from 'react'
import { Slider } from '@electricui/components-desktop-blueprint'
import { Statistic, Statistics } from '@electricui/components-desktop-blueprint'
import { ProgressBar } from '@electricui/components-desktop-blueprint'
import { HTMLTable } from '@blueprintjs/core'
import { DataSourcePrinter } from '@electricui/components-desktop-charts'
import { HelpPopover } from 'src/application/components/HelpPopover/HelpPopover'
import { Callout } from '@blueprintjs/core'
import { Switch } from '@electricui/components-desktop-blueprint'
import { NumberInput } from '@electricui/components-desktop-blueprint'

const layoutDescription = `
      SDInfo Controls
    `

const sdEnableDS = new MessageDataSource('sde')

export const LoggingPage = (props: RouteComponentProps) => {
  return (
    <React.Fragment>
      <Composition areas={layoutDescription} gap={10} autoCols="1fr">
        {Areas => (
          <React.Fragment>
            <Areas.SDInfo>
              <Card>
                <h1>SD Card Info</h1>
                <Statistics>
                    <Statistic
                        label="Latitude"
                        accessor="sdhz"
                        suffix="Hz"
                        precision={4}
                        color="#9f7ef7"
                    />
                </Statistics>
              </Card>
            </Areas.SDInfo>
            <Areas.Controls>
              <Card>
                <Switch
                unchecked={0}
                checked={1}
                accessor="sde"
                >
                    Enable SD Logging
                </Switch>
                <NumberInput
                    accessor="sdr"
                    min={1}
                    max={100}
                    stepSize={1}
                />
              </Card>
            </Areas.Controls>
          </React.Fragment>
        )}
      </Composition>
    </React.Fragment>
  )
}
