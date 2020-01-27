namespace SystemSim
{
    partial class MainFrame
    {
        /// <summary>
        /// 必要なデザイナー変数です。
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 使用中のリソースをすべてクリーンアップします。
        /// </summary>
        /// <param name="disposing">マネージ リソースを破棄する場合は true を指定し、その他の場合は false を指定します。</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows フォーム デザイナーで生成されたコード

        /// <summary>
        /// デザイナー サポートに必要なメソッドです。このメソッドの内容を
        /// コード エディターで変更しないでください。
        /// </summary>
        private void InitializeComponent()
        {
			this.components = new System.ComponentModel.Container();
			this.flowLayoutPanel1 = new System.Windows.Forms.FlowLayoutPanel();
			this.RunBtn = new System.Windows.Forms.Button();
			this.StopBtn = new System.Windows.Forms.Button();
			this.tabControl1 = new System.Windows.Forms.TabControl();
			this.tabPage1 = new System.Windows.Forms.TabPage();
			this.tabPage2 = new System.Windows.Forms.TabPage();
			this.dataGridView2 = new System.Windows.Forms.DataGridView();
			this.nameDataGridViewTextBoxColumn = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.valueDataGridViewTextBoxColumn = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.dataDataGridViewTextBoxColumn1 = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.packetViewItemBindingSource = new System.Windows.Forms.BindingSource(this.components);
			this.splitter1 = new System.Windows.Forms.Splitter();
			this.dataGridView1 = new System.Windows.Forms.DataGridView();
			this.ethernetLogItemBindingSource = new System.Windows.Forms.BindingSource(this.components);
			this.timer1 = new System.Windows.Forms.Timer(this.components);
			this.timeDataGridViewTextBoxColumn = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.destinationDataGridViewTextBoxColumn = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.sourceDataGridViewTextBoxColumn = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.typeDataGridViewTextBoxColumn = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.dataDataGridViewTextBoxColumn = new System.Windows.Forms.DataGridViewTextBoxColumn();
			this.flowLayoutPanel1.SuspendLayout();
			this.tabControl1.SuspendLayout();
			this.tabPage1.SuspendLayout();
			this.tabPage2.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.dataGridView2)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.packetViewItemBindingSource)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.dataGridView1)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.ethernetLogItemBindingSource)).BeginInit();
			this.SuspendLayout();
			// 
			// flowLayoutPanel1
			// 
			this.flowLayoutPanel1.AutoSize = true;
			this.flowLayoutPanel1.Controls.Add(this.RunBtn);
			this.flowLayoutPanel1.Controls.Add(this.StopBtn);
			this.flowLayoutPanel1.Dock = System.Windows.Forms.DockStyle.Top;
			this.flowLayoutPanel1.Location = new System.Drawing.Point(3, 3);
			this.flowLayoutPanel1.Name = "flowLayoutPanel1";
			this.flowLayoutPanel1.Size = new System.Drawing.Size(901, 29);
			this.flowLayoutPanel1.TabIndex = 1;
			// 
			// RunBtn
			// 
			this.RunBtn.Location = new System.Drawing.Point(3, 3);
			this.RunBtn.Name = "RunBtn";
			this.RunBtn.Size = new System.Drawing.Size(87, 23);
			this.RunBtn.TabIndex = 0;
			this.RunBtn.Text = "実行";
			this.RunBtn.UseVisualStyleBackColor = true;
			this.RunBtn.Click += new System.EventHandler(this.RunBtn_Click);
			// 
			// StopBtn
			// 
			this.StopBtn.Location = new System.Drawing.Point(96, 3);
			this.StopBtn.Name = "StopBtn";
			this.StopBtn.Size = new System.Drawing.Size(87, 23);
			this.StopBtn.TabIndex = 1;
			this.StopBtn.Text = "停止";
			this.StopBtn.UseVisualStyleBackColor = true;
			this.StopBtn.Click += new System.EventHandler(this.StopBtn_Click);
			// 
			// tabControl1
			// 
			this.tabControl1.Controls.Add(this.tabPage1);
			this.tabControl1.Controls.Add(this.tabPage2);
			this.tabControl1.Dock = System.Windows.Forms.DockStyle.Fill;
			this.tabControl1.Location = new System.Drawing.Point(0, 0);
			this.tabControl1.Name = "tabControl1";
			this.tabControl1.SelectedIndex = 0;
			this.tabControl1.Size = new System.Drawing.Size(915, 411);
			this.tabControl1.TabIndex = 2;
			this.tabControl1.SelectedIndexChanged += new System.EventHandler(this.TabControl1_SelectedIndexChanged);
			// 
			// tabPage1
			// 
			this.tabPage1.Controls.Add(this.flowLayoutPanel1);
			this.tabPage1.Location = new System.Drawing.Point(4, 22);
			this.tabPage1.Name = "tabPage1";
			this.tabPage1.Padding = new System.Windows.Forms.Padding(3);
			this.tabPage1.Size = new System.Drawing.Size(907, 385);
			this.tabPage1.TabIndex = 0;
			this.tabPage1.Text = "設定";
			this.tabPage1.UseVisualStyleBackColor = true;
			// 
			// tabPage2
			// 
			this.tabPage2.Controls.Add(this.dataGridView2);
			this.tabPage2.Location = new System.Drawing.Point(4, 22);
			this.tabPage2.Name = "tabPage2";
			this.tabPage2.Padding = new System.Windows.Forms.Padding(3);
			this.tabPage2.Size = new System.Drawing.Size(907, 385);
			this.tabPage2.TabIndex = 1;
			this.tabPage2.Text = "パケット解析";
			this.tabPage2.UseVisualStyleBackColor = true;
			// 
			// dataGridView2
			// 
			this.dataGridView2.AllowUserToAddRows = false;
			this.dataGridView2.AllowUserToDeleteRows = false;
			this.dataGridView2.AllowUserToResizeRows = false;
			this.dataGridView2.AutoGenerateColumns = false;
			this.dataGridView2.BorderStyle = System.Windows.Forms.BorderStyle.None;
			this.dataGridView2.CellBorderStyle = System.Windows.Forms.DataGridViewCellBorderStyle.None;
			this.dataGridView2.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
			this.dataGridView2.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.nameDataGridViewTextBoxColumn,
            this.valueDataGridViewTextBoxColumn,
            this.dataDataGridViewTextBoxColumn1});
			this.dataGridView2.DataSource = this.packetViewItemBindingSource;
			this.dataGridView2.Dock = System.Windows.Forms.DockStyle.Fill;
			this.dataGridView2.Location = new System.Drawing.Point(3, 3);
			this.dataGridView2.Name = "dataGridView2";
			this.dataGridView2.ReadOnly = true;
			this.dataGridView2.RowHeadersVisible = false;
			this.dataGridView2.RowTemplate.Height = 21;
			this.dataGridView2.Size = new System.Drawing.Size(901, 379);
			this.dataGridView2.TabIndex = 0;
			// 
			// nameDataGridViewTextBoxColumn
			// 
			this.nameDataGridViewTextBoxColumn.DataPropertyName = "Name";
			this.nameDataGridViewTextBoxColumn.HeaderText = "名称";
			this.nameDataGridViewTextBoxColumn.Name = "nameDataGridViewTextBoxColumn";
			this.nameDataGridViewTextBoxColumn.ReadOnly = true;
			this.nameDataGridViewTextBoxColumn.Width = 200;
			// 
			// valueDataGridViewTextBoxColumn
			// 
			this.valueDataGridViewTextBoxColumn.DataPropertyName = "Value";
			this.valueDataGridViewTextBoxColumn.HeaderText = "値";
			this.valueDataGridViewTextBoxColumn.Name = "valueDataGridViewTextBoxColumn";
			this.valueDataGridViewTextBoxColumn.ReadOnly = true;
			this.valueDataGridViewTextBoxColumn.Width = 200;
			// 
			// dataDataGridViewTextBoxColumn1
			// 
			this.dataDataGridViewTextBoxColumn1.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.Fill;
			this.dataDataGridViewTextBoxColumn1.DataPropertyName = "Data";
			this.dataDataGridViewTextBoxColumn1.HeaderText = "データ";
			this.dataDataGridViewTextBoxColumn1.Name = "dataDataGridViewTextBoxColumn1";
			this.dataDataGridViewTextBoxColumn1.ReadOnly = true;
			// 
			// packetViewItemBindingSource
			// 
			this.packetViewItemBindingSource.DataSource = typeof(SystemSim.PacketViewItem);
			// 
			// splitter1
			// 
			this.splitter1.Cursor = System.Windows.Forms.Cursors.HSplit;
			this.splitter1.Dock = System.Windows.Forms.DockStyle.Bottom;
			this.splitter1.Location = new System.Drawing.Point(0, 408);
			this.splitter1.Name = "splitter1";
			this.splitter1.Size = new System.Drawing.Size(915, 3);
			this.splitter1.TabIndex = 5;
			this.splitter1.TabStop = false;
			// 
			// dataGridView1
			// 
			this.dataGridView1.AllowUserToAddRows = false;
			this.dataGridView1.AllowUserToDeleteRows = false;
			this.dataGridView1.AllowUserToResizeRows = false;
			this.dataGridView1.AutoGenerateColumns = false;
			this.dataGridView1.BorderStyle = System.Windows.Forms.BorderStyle.None;
			this.dataGridView1.CellBorderStyle = System.Windows.Forms.DataGridViewCellBorderStyle.None;
			this.dataGridView1.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
			this.dataGridView1.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.timeDataGridViewTextBoxColumn,
            this.destinationDataGridViewTextBoxColumn,
            this.sourceDataGridViewTextBoxColumn,
            this.typeDataGridViewTextBoxColumn,
            this.dataDataGridViewTextBoxColumn});
			this.dataGridView1.DataSource = this.ethernetLogItemBindingSource;
			this.dataGridView1.Dock = System.Windows.Forms.DockStyle.Bottom;
			this.dataGridView1.Location = new System.Drawing.Point(0, 411);
			this.dataGridView1.Name = "dataGridView1";
			this.dataGridView1.ReadOnly = true;
			this.dataGridView1.RowHeadersVisible = false;
			this.dataGridView1.RowTemplate.Height = 21;
			this.dataGridView1.Size = new System.Drawing.Size(915, 150);
			this.dataGridView1.TabIndex = 6;
			this.dataGridView1.CurrentCellChanged += new System.EventHandler(this.DataGridView1_CurrentCellChanged);
			// 
			// ethernetLogItemBindingSource
			// 
			this.ethernetLogItemBindingSource.DataSource = typeof(SimEnv.EthernetLogItem);
			// 
			// timer1
			// 
			this.timer1.Tick += new System.EventHandler(this.Timer1_Tick);
			// 
			// timeDataGridViewTextBoxColumn
			// 
			this.timeDataGridViewTextBoxColumn.DataPropertyName = "Time";
			this.timeDataGridViewTextBoxColumn.HeaderText = "時刻";
			this.timeDataGridViewTextBoxColumn.Name = "timeDataGridViewTextBoxColumn";
			this.timeDataGridViewTextBoxColumn.ReadOnly = true;
			this.timeDataGridViewTextBoxColumn.Width = 160;
			// 
			// destinationDataGridViewTextBoxColumn
			// 
			this.destinationDataGridViewTextBoxColumn.DataPropertyName = "Destination";
			this.destinationDataGridViewTextBoxColumn.HeaderText = "宛先";
			this.destinationDataGridViewTextBoxColumn.Name = "destinationDataGridViewTextBoxColumn";
			this.destinationDataGridViewTextBoxColumn.ReadOnly = true;
			this.destinationDataGridViewTextBoxColumn.Width = 120;
			// 
			// sourceDataGridViewTextBoxColumn
			// 
			this.sourceDataGridViewTextBoxColumn.DataPropertyName = "Source";
			this.sourceDataGridViewTextBoxColumn.HeaderText = "送信元";
			this.sourceDataGridViewTextBoxColumn.Name = "sourceDataGridViewTextBoxColumn";
			this.sourceDataGridViewTextBoxColumn.ReadOnly = true;
			this.sourceDataGridViewTextBoxColumn.Width = 120;
			// 
			// typeDataGridViewTextBoxColumn
			// 
			this.typeDataGridViewTextBoxColumn.DataPropertyName = "Type";
			this.typeDataGridViewTextBoxColumn.HeaderText = "タイプ";
			this.typeDataGridViewTextBoxColumn.Name = "typeDataGridViewTextBoxColumn";
			this.typeDataGridViewTextBoxColumn.ReadOnly = true;
			this.typeDataGridViewTextBoxColumn.Width = 60;
			// 
			// dataDataGridViewTextBoxColumn
			// 
			this.dataDataGridViewTextBoxColumn.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.Fill;
			this.dataDataGridViewTextBoxColumn.DataPropertyName = "Data";
			this.dataDataGridViewTextBoxColumn.HeaderText = "データ";
			this.dataDataGridViewTextBoxColumn.Name = "dataDataGridViewTextBoxColumn";
			this.dataDataGridViewTextBoxColumn.ReadOnly = true;
			// 
			// MainFrame
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(915, 561);
			this.Controls.Add(this.splitter1);
			this.Controls.Add(this.tabControl1);
			this.Controls.Add(this.dataGridView1);
			this.Name = "MainFrame";
			this.Text = "ユニットテスト";
			this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.MainFrame_FormClosing);
			this.Load += new System.EventHandler(this.MainFrame_Load);
			this.flowLayoutPanel1.ResumeLayout(false);
			this.tabControl1.ResumeLayout(false);
			this.tabPage1.ResumeLayout(false);
			this.tabPage1.PerformLayout();
			this.tabPage2.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.dataGridView2)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.packetViewItemBindingSource)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.dataGridView1)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.ethernetLogItemBindingSource)).EndInit();
			this.ResumeLayout(false);

        }

        #endregion

		private System.Windows.Forms.FlowLayoutPanel flowLayoutPanel1;
		private System.Windows.Forms.Button RunBtn;
		private System.Windows.Forms.TabControl tabControl1;
		private System.Windows.Forms.TabPage tabPage1;
		private System.Windows.Forms.Button StopBtn;
		private System.Windows.Forms.Splitter splitter1;
		private System.Windows.Forms.DataGridView dataGridView1;
		private System.Windows.Forms.BindingSource ethernetLogItemBindingSource;
		private System.Windows.Forms.TabPage tabPage2;
		private System.Windows.Forms.DataGridView dataGridView2;
		private System.Windows.Forms.BindingSource packetViewItemBindingSource;
		private System.Windows.Forms.Timer timer1;
		private System.Windows.Forms.DataGridViewTextBoxColumn nameDataGridViewTextBoxColumn;
		private System.Windows.Forms.DataGridViewTextBoxColumn valueDataGridViewTextBoxColumn;
		private System.Windows.Forms.DataGridViewTextBoxColumn dataDataGridViewTextBoxColumn1;
		private System.Windows.Forms.DataGridViewTextBoxColumn timeDataGridViewTextBoxColumn;
		private System.Windows.Forms.DataGridViewTextBoxColumn destinationDataGridViewTextBoxColumn;
		private System.Windows.Forms.DataGridViewTextBoxColumn sourceDataGridViewTextBoxColumn;
		private System.Windows.Forms.DataGridViewTextBoxColumn typeDataGridViewTextBoxColumn;
		private System.Windows.Forms.DataGridViewTextBoxColumn dataDataGridViewTextBoxColumn;
	}
}

