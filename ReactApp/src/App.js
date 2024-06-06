
import LS2Request from '@enact/webos/LS2Request';
import 'bootstrap/dist/css/bootstrap.css';
import Button from 'react-bootstrap/Button';
import Form from 'react-bootstrap/Form';
import InputGroup from 'react-bootstrap/InputGroup';
import React, {useState} from "react";
import {DropdownButton, ToggleButton, Dropdown} from "react-bootstrap";

let webOSBridge = new LS2Request();
let glEffectsTitle = ["No GlEffect", ]
let glEffectsButton = ["None", ]

function ObjectDetectButton(props) {
  return (
      <>
        <InputGroup className="mb-3">
          <Button variant="outline-secondary" id="button-addon1" onClick={props.onClick}>
            Use Local File
          </Button>
          <Form.Control
              onChange={props.onChange}
              placeholder="Input URI"
              aria-label="Example text with button addon"
              aria-describedby="basic-addon1"
              value={props.uri}
          />
        </InputGroup>
      </>
  )
}

function App() {
    let [uri, setUri] = useState("")
    let [useObjectDetection, setUseObjectDetection] = useState(false)
    let [glEffect, setGlEffect] = useState(0)

    return (
        <div className="App">
            <header className="App-header">
                <ToggleButton id="tbg-btn-1" type="checkbox" value={1} checked={useObjectDetection} onChange={() => setUseObjectDetection(!useObjectDetection)} >
                    Use Object Detection
                </ToggleButton>
                <DropdownButton
                    variant="outline-secondary"
                    title={glEffectsTitle[glEffect]}
                    id="dropdown-basic-button"
                >
                    {glEffectsButton.map((value, index) => {
                        return (
                            <Dropdown.Item href="#"
                                           active={glEffect === index}
                                           onClick={() => setGlEffect(index)}>
                                {value}
                            </Dropdown.Item>
                        )
                    })}
                </DropdownButton>
                <ObjectDetectButton onClick={() => {
                    const lsRequest = {
                        "service": "luna://com.balance.app.service",
                        "method": "file",
                        "parameters": {
                            "uri": uri,
                            "objectDetection": useObjectDetection,
                            "glEffect": glEffect
                        },
                        "onSuccess": (ignored) => {
                        },
                        "onFailure": (ignored) => {
                        }
                    };
                    webOSBridge.send(lsRequest)
                    setUri("")
                }} onChange={(e) => {
                    setUri(e.target.value)
                }} uri={uri}
                />
                <Button variant="outline-secondary" id="button-addon1" onClick={() => {
                    const lsRequest = {
                        "service": "luna://com.balance.app.service",
                        "method": "camera",
                        "parameters": {
                            "objectDetection": useObjectDetection,
                            "glEffect": glEffect
                        },
                        "onSuccess": (ignored) => {
                        },
                        "onFailure": (ignored) => {
                        }
                    };
                    webOSBridge.send(lsRequest)
                }}>
                    Use Camera
                </Button>
            </header>
        </div>
    );
}

export default App;
