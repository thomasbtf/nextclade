import { pickBy } from 'lodash'

import { CLADES } from 'src/algorithms/clades'
import { parseSequences } from 'src/algorithms/parseSequences'
import { isSequenceInClade } from 'src/algorithms/isSequenceInClade'
import { analyzeSeq } from 'src/algorithms/analyzeSeq'

export interface AlgorithmParams {
  rootSeq: string
  input: string
}

export interface CladeLocation {
  pos: number
  allele: string
}

export interface Clades {
  [key: string]: CladeLocation[]
}

export interface AnalyzeSeqResult {
  seqName: string
  clades: Clades
}

export interface AlgorithmResult {
  result: AnalyzeSeqResult[]
}

export async function run({ input, rootSeq }: AlgorithmParams): Promise<AlgorithmResult> {
  const lines = input.split('\n')
  const parsedSequences = parseSequences(lines)

  const result = Object.entries(parsedSequences)
    .map(([seqName, seq]) => {
      const { mutations, insertions, deletions, alnStart, alnEnd } = analyzeSeq(seq, rootSeq)
      const clades = pickBy(CLADES, (clade) => isSequenceInClade(clade, mutations, rootSeq))
      return { seqName, clades }
    })
    .filter(({ clades }) => Object.keys(clades).length !== 0)

  return { result }
}