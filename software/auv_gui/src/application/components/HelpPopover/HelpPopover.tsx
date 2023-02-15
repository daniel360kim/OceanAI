import React from 'react'
import { IconNames } from '@blueprintjs/icons'
import {
    Icon,
    Popover,
    Intent,
    PopoverPosition
} from '@blueprintjs/core'

type HelpProps = {
    title: string
    children: any
}
export const HelpPopover = (props: HelpProps) => {
    return (
        <React.Fragment>
            <Popover
                interactionKind="hover"
                hoverOpenDelay={200}
                content={
                    <div style={{ padding: '1em' }}>
                        <h3>{props.title}</h3>
                        <p>{props.children}</p>
                    </div>
                }
                >
                <Icon icon={IconNames.HELP} intent={Intent.WARNING} iconSize={20} />
            </Popover>
        </React.Fragment>
    )
}