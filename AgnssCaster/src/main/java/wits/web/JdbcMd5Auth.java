package wits.web;

import io.vertx.ext.auth.jdbc.JDBCAuth;
import io.vertx.ext.jdbc.JDBCClient;


public class JdbcMd5Auth {
	
	public static String USER_TABLE = "User";
	
	public static JDBCAuth create(JDBCClient sqlClient) {
		
		JDBCAuth authProvider = JDBCAuth.create(sqlClient);
		
		String sql = "select password,password from " + USER_TABLE + " where username=?";
		authProvider.setAuthenticationQuery(sql);
		authProvider.setHashStrategy(new Md5HashStrategy());
		
		return authProvider;
	}
	
	
}
