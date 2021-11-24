import React, { useMemo, useState } from 'react'

import { Button, Col, Label, Row } from 'reactstrap'

import { useTranslationSafe } from 'src/helpers/useTranslationSafe'
import { ButtonContainer, ColFlexHorizontal, ColFlexVertical, Footnote, Form, RowFill } from './FilePickerStyles'
import { TextInputMonospace } from './TextInputMonospace'

export interface TabPanelUrlProps {
  exampleUrl: string

  onConfirm(url: string): void
}

export function TabPanelUrl({ exampleUrl, onConfirm }: TabPanelUrlProps) {
  const { t } = useTranslationSafe()
  const [url, setUrl] = useState<string>('')
  const hasUrl = url.length > 0
  const change = (e: React.ChangeEvent<HTMLInputElement>) => setUrl(e.target.value)
  const clear = () => setUrl('')
  const confirm = () => onConfirm(url)
  const placeholder = useMemo(() => t('For example: {{exampleUrl}}', { exampleUrl }), [t, exampleUrl])

  return (
    <Form>
      <RowFill noGutters>
        <ColFlexVertical>
          <Row noGutters>
            <Col className="d-flex">
              <Label className="mr-auto" htmlFor="tree-url-text-input">
                {t('Enter URL to a file to fetch')}
              </Label>
            </Col>
          </Row>

          <RowFill noGutters>
            <ColFlexVertical>
              <TextInputMonospace
                wrap="hard"
                id="tree-url-text-input"
                className="flex-grow-1"
                type="textarea"
                placeholder={placeholder}
                autoComplete="off"
                autoCorrect="off"
                autoCapitalize="off"
                spellCheck="false"
                data-gramm="false"
                data-gramm_editor="false"
                value={url}
                onChange={change}
              />
            </ColFlexVertical>
          </RowFill>

          <Row>
            <ColFlexHorizontal>
              <Footnote>{t('*Make sure this file is publicly accessible and CORS is enabled on your server')}</Footnote>
            </ColFlexHorizontal>
          </Row>

          <Row noGutters>
            <ColFlexHorizontal>
              <ButtonContainer>
                <Button
                  disabled={!hasUrl}
                  type="button"
                  color="secondary"
                  title={t('Clear the URL text field')}
                  onClick={clear}
                >
                  {t('Clear')}
                </Button>

                <Button
                  disabled={!hasUrl}
                  type="button"
                  color="primary"
                  title={hasUrl ? 'Start downloading this file' : 'Provide a URL before downloading is possible'}
                  onClick={confirm}
                >
                  {t('OK')}
                </Button>
              </ButtonContainer>
            </ColFlexHorizontal>
          </Row>
        </ColFlexVertical>
      </RowFill>
    </Form>
  )
}
