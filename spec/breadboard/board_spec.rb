require_relative 'board'

describe Breadboard, hardware: true do
  subject(:board) { described_class.new params, links }

  describe 'digital_io' do
    before { board.upload 'bin/test-firmware.bin' }

    context 'with the input pin pulled down' do
      let(:params) { { termination: :pulldown } }

      it 'is driven as expected' do
        expect(board.events).to eq [
          'driven low',  'read low',
          'driven high', 'read high',
          'not driven',  'read low'
        ]
      end
    end

    context 'with the input pin pulled up' do
      let(:params) { { termination: :pullup } }

      it 'is driven as expected' do
        expect(board.events).to eq [
          'driven low',  'read low',
          'driven high', 'read high',
          'not driven',  'read high'
        ]
      end
    end

    context 'with the input pin in repeater mode' do
      let(:params) { { termination: :repeater } }

      it 'is driven as expected' do
        expect(board.events).to eq [
          'driven low',  'read low',
          'driven high', 'read high',
          'not driven',  'read high'
        ]
      end
    end
  end
end
