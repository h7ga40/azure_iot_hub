using MicroServer.Extensions;
using System;
using System.Text;

namespace MicroServer.Net.Dns
{
	#region RFC Specification

	/* Description: Object that represents a DNSv4 message as defined in
    *               RFC 2131.
    *               
    *  The following diagram illustrates the header format of DNS messages sent
    *  between server and clients as defined by 4.1.1:
    *
    *    						       1  1  1  1  1  1
	*    0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5
    *  +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    *  |                      ID                       |
    *  +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    *  |QR|   Opcode  |AA|TC|RD|RA|   Z    |   RCODE   |
    *  +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    *  |                    QDCOUNT                    |
    *  +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    *  |                    ANCOUNT                    |
    *  +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    *  |                    NSCOUNT                    |
    *  +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    *  |                    ARCOUNT                    |
    *  +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    *  
    *  where:
    *  ID              A 16 bit identifier assigned by the program that
	*  			    generates any kind of query.  This identifier is copied
	*  			    the corresponding reply and can be used by the requester
	*  			    to match up replies to outstanding queries.
    *  			    
    *  QR              A one bit field that specifies whether this message is a
	*  			    query (0), or a response (1).
    *  
    *  OPCODE       A four bit field that specifies kind of query in this
	*  			    message.  This value is set by the originator of a query
	*  			    and copied into the response.  The values are:
    *  
	*  			    0               a standard query (QUERY)
	*  			    1               an inverse query (IQUERY)  
	*  			    2               a server status request (STATUS)
	*  			    3-15            reserved for future use
    *  
    *  AA           Authoritative Answer - this bit is valid in responses,
	*  			    and specifies that the responding name server is an
	*  			    authority for the domain name in question section.
    *  
	*  			    Note that the contents of the answer section may have
	*  			    multiple owner names because of aliases.  The AA bit
	*  			    corresponds to the name which matches the query name, or
	*  			    the first owner name in the answer section.
    *  
    *  TC           TrunCation - specifies that this message was truncated
	*  			    due to length greater than that permitted on the
	*  			    transmission channel.
    *  
    *  RD           Recursion Desired - this bit may be set in a query and
	*  			    is copied into the response.  If RD is set, it directs
	*  			    the name server to pursue the query recursively.
	*  			    Recursive query support is optional.
    *  
    *  RA           Recursion Available - this be is set or cleared in a
	*  			    response, and denotes whether recursive query support is
	*  			    available in the name server.
    *  
    *  Z            Reserved for future use.  Must be zero in all queries
	*  			    and responses.
    *  
    *  RCODE        Response code - this 4 bit field is set as part of
	*  			    responses.  The values have the following
	*  			    interpretation:
    *  
	*  			    0               No error condition
	*  			    1               Format error - The name server was
	*  							    unable to interpret the query.
	*  			    2               Server failure - The name server was
	*  							    unable to process this query due to a
	*  							    problem with the name server.
	*  			    3               Name Error - Meaningful only for
	*  							    responses from an authoritative name
	*  							    server, this code signifies that the
	*  							    domain name referenced in the query does
	*  							    not exist.
	*  			    4               Not Implemented - The name server does
	*  							    not support the requested kind of query.
	*  			    5               Refused - The name server refuses to
	*  							    perform the specified operation for
	*  							    policy reasons.  For example, a name
	*  							    server may not wish to provide the
	*  							    information to the particular requester,
	*  							    or a name server may not wish to perform
	*  							    a particular operation (e.g., zone
	*  							    transfer) for particular data.
	*  			    6-15            Reserved for future use.
    *  
    *  QDCOUNT      an unsigned 16 bit integer specifying the number of
	*  			    entries in the question section.
    *  
    *  ANCOUNT      an unsigned 16 bit integer specifying the number of
	*  			    resource records in the answer section.
    *  
    *  NSCOUNT      an unsigned 16 bit integer specifying the number of name
	*  			    server resource records in the authority records
	*  			    section.
    *  
    *  ARCOUNT      an unsigned 16 bit integer specifying the number of
	*  			    resource records in the additional records section.
    *  
    */

	#endregion RFC Specification

	public class DnsMessage
	{
		#region Private Properties

		private static readonly Random RANDOM = new Random();

		#endregion Private Properties

		#region Constructors

		public DnsMessage()
		{

			Timestamp = DateTime.Now;
			Header = new Header {
				TransactionId = (ushort)RANDOM.Next(UInt16.MaxValue),
				QR = false,
				OperationCode = OperationCode.Query
			};

			Questions = new Question[0];
			Answers = new Answer[0];
			Authorities = new Authority[0];
			Additionals = new Additional[0];

		}

		public DnsMessage(byte[] data)
		{
			Timestamp = DateTime.Now;
			var byteReader = new DnsReader(data);

			// Header
			Header = new Header(byteReader);

			// Question, Answer, Authority, Additional Counts
			Questions = new Question[QuestionRecordCount];
			Answers = new Answer[AnswerRecordCount];
			Authorities = new Authority[AuthorityRecordCount];
			Additionals = new Additional[AdditionalRecordCount];

			// Read Records
			for (var i = 0; i < QuestionRecordCount; i++)
				Questions[i] = new Question(byteReader);

			for (var i = 0; i < AnswerRecordCount; i++)
				Answers[i] = new Answer(byteReader);

			for (var i = 0; i < AuthorityRecordCount; i++)
				Authorities[i] = new Authority(byteReader);

			for (var i = 0; i < AdditionalRecordCount; i++)
				Additionals[i] = new Additional(byteReader);

		}

		#endregion Constructors

		#region Public Properties

		/// <summary>
		/// TimeStamp when cached.
		/// </summary>
		public DateTime Timestamp;

		/// <summary>
		/// Header record
		/// </summary>
		public Header Header;

		/// <summary>
		/// List of Question records.
		/// </summary>
		public Question[] Questions;

		/// <summary>
		/// List of AnswerRR records.
		/// </summary>
		public Answer[] Answers;

		/// <summary>
		/// List of AuthorityRR records.
		/// </summary>
		public Authority[] Authorities;

		/// <summary>
		/// List of AdditionalRR records.
		/// </summary>
		public Additional[] Additionals;

		/// <summary>
		/// An identifier assigned by the service.
		/// </summary>
		public ushort TransactionId {
			get => Header.TransactionId;
			set => Header.TransactionId = value;
		}

		/// <summary>
		/// the number of entries in the question section.
		/// </summary>
		public int QuestionRecordCount {
			get => Header.QDCOUNT;
			set => Header.QDCOUNT = (ushort)value;
		}

		/// <summary>
		/// The number of resource records in the answer section.
		/// </summary>
		public int AnswerRecordCount {
			get => Header.ANCOUNT;
			set => Header.ANCOUNT = (ushort)value;
		}

		/// <summary>
		/// The number of name server resource records in the authority records section.
		/// </summary>
		public int AuthorityRecordCount {
			get => Header.NSCOUNT;
			set => Header.NSCOUNT = (ushort)value;
		}

		/// <summary>
		/// The number of resource records in the additional records section.
		/// </summary>
		public int AdditionalRecordCount {
			get => Header.ARCOUNT;
			set => Header.ARCOUNT = (ushort)value;
		}

		/// <summary>
		/// Is authoritative answer (AA).
		/// </summary>
		public bool IsAuthoritiveAnswer {
			get => Header.AuthoritiveAnswer;
			set => Header.AuthoritiveAnswer = value;
		}

		/// <summary>
		/// Is Query(true) or Response(false) flag (QR).
		/// </summary>
		public bool IsQuery {
			get => Header.QR;
			set => Header.QR = value;
		}

		/// <summary>
		/// Is Recursion Available (AA).
		/// </summary>
		public bool IsRecursionAvailable {
			get => Header.RecursionAvailable;
			set => Header.RecursionAvailable = value;
		}

		/// <summary>
		/// Is Recursion Desired (RD).
		/// </summary>
		public bool IsRecursionDesired {
			get => Header.RecursionDesired;
			set => Header.RecursionDesired = value;
		}

		/// <summary>
		/// TrunCation (TC).
		/// </summary>
		public bool IsTruncated {
			get => Header.Truncated;
			set => Header.Truncated = value;
		}

		/// <summary>
		/// Specifies type of query (OPCODE).
		/// </summary>
		public OperationCode OperationCode {
			get => Header.OperationCode;
			set => Header.OperationCode = value;
		}

		/// <summary>
		/// Specifies the response code (RCODE).
		/// </summary>
		public ReturnCode ResponseCode {
			get => Header.ResponseCode;
			set => Header.ResponseCode = value;
		}

		#endregion Public Properties

		#region Methods

		public byte[] ToArray()
		{
			var byteWriter = new DnsWriter();

			// Header
			Header.WriteBytes(byteWriter);

			// Question, Answer, Authority, Additional Counts
			byteWriter.Write((short)QuestionRecordCount);
			byteWriter.Write((short)AnswerRecordCount);
			byteWriter.Write((short)AuthorityRecordCount);
			byteWriter.Write((short)AdditionalRecordCount);

			// Write Records
			foreach (var record in Questions) {
				record.Write(byteWriter);
			}

			foreach (var record in Answers) {
				record.Write(byteWriter);
			}

			foreach (var record in Authorities) {
				record.Write(byteWriter);
			}

			foreach (var record in Additionals) {
				record.Write(byteWriter);
			}

			return byteWriter.GetBytes();
		}

		public override string ToString()
		{
			var sb = new StringBuilder();

			sb.AppendLine();
			sb.AppendLine("  DNS PACKET");
			sb.AppendLine(String.Concat("  Message Timestamp              : ", Timestamp.ToLocalTime().ToString()));
			sb.AppendLine(String.Concat("  Transaction Id (ID)            : ", TransactionId.ToHexString("0x")));
			sb.AppendLine(String.Concat("  Is Query (RQ)                  : ", IsQuery.ToString()));
			sb.AppendLine(String.Concat("  Operation Code (Opcode)        : ", OperationCode.ToString()));
			sb.AppendLine(String.Concat("  Is Authoritive Answer (AA)     : ", IsAuthoritiveAnswer.ToString()));
			sb.AppendLine(String.Concat("  Is Truncated (TC)              : ", IsTruncated.ToString()));
			sb.AppendLine(String.Concat("  Is Recursion Desired (RD)      : ", IsRecursionDesired.ToString()));
			sb.AppendLine(String.Concat("  Is Recursion Available (RA)    : ", IsRecursionAvailable.ToString()));
			sb.AppendLine(String.Concat("  Response Code (RCODE)          : ", ResponseCode.ToString()));

			sb.AppendLine(String.Concat("  Question Count (QDCOUNT)       : ", QuestionRecordCount.ToString()));
			foreach (var question in Questions) {
				var record = question as Question;
				if (record != null) {
					sb.AppendLine(String.Concat("  -Question Record (Name)        : ", record.Domain.ToString()));
					sb.AppendLine(String.Concat("  -Class                         : ", record.Class.ToString()));
					sb.AppendLine(String.Concat("  -Type                          : ", record.Type.ToString()));
				}
			}

			sb.AppendLine(String.Concat("  Answer Count (ANCOUNT)         : ", AnswerRecordCount.ToString()));
			foreach (var answer in Answers) {
				var record = answer as Answer;
				if (record != null) {
					sb.AppendLine(String.Concat("  -Answer Record (Name)          : ", record.Domain.ToString()));
					sb.AppendLine(String.Concat("  -Class                         : ", record.Class.ToString()));
					sb.AppendLine(String.Concat("  -Type                          : ", record.Type.ToString()));
					sb.AppendLine(String.Concat("  -Ttl                           : ", record.Ttl.ToString()));
					sb.AppendLine(String.Concat("  -Record                        : ", record.Record.ToString()));
				}
			}

			sb.AppendLine(String.Concat("  Additional Count (ARCOUNT)     : ", AdditionalRecordCount.ToString()));
			foreach (var additional in Additionals) {
				var record = additional as Additional;
				if (record != null) {
					sb.AppendLine(String.Concat("  -Additional Record (Name)      : ", record.Domain.ToString()));
					sb.AppendLine(String.Concat("  -Class                         : ", record.Class.ToString()));
					sb.AppendLine(String.Concat("  -Type                          : ", record.Type.ToString()));
					sb.AppendLine(String.Concat("  -Ttl                           : ", record.Ttl.ToString()));
					sb.AppendLine(String.Concat("  -Record                        : ", record.Record.ToString()));
				}
			}

			sb.AppendLine(String.Concat("  Authority Count (NACOUNT)      : ", AuthorityRecordCount.ToString()));
			foreach (var authority in Authorities) {
				var record = authority as Authority;
				if (record != null) {
					sb.AppendLine(String.Concat("  -Authority Record (Name)       : ", record.Domain.ToString()));
					sb.AppendLine(String.Concat("  -Class                         : ", record.Class.ToString()));
					sb.AppendLine(String.Concat("  -Type                          : ", record.Type.ToString()));
					sb.AppendLine(String.Concat("  -Ttl                           : ", record.Ttl.ToString()));
					sb.AppendLine(String.Concat("  -Record                        : ", record.Record.ToString()));
				}
			}

			sb.AppendLine();
			return sb.ToString();
		}

		#endregion Methods
	}
}
