package wits.web;

import java.io.UnsupportedEncodingException;
import java.math.BigInteger;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;

import io.vertx.core.json.JsonArray;
import io.vertx.ext.auth.jdbc.JDBCHashStrategy;

public class Md5HashStrategy implements JDBCHashStrategy {

	@Override
	public String computeHash(String password, String salt) {
		return md5(password);
	}

	@Override
	public String getHashedStoredPwd(JsonArray row) {
		return row.getString(0);
	}

	@Override
	public String getSalt(JsonArray row) {
		return "";
	}

	public static String md5(String content) {
		try {
			MessageDigest messageDigest = MessageDigest.getInstance("MD5");
			byte[] digest = messageDigest.digest(content.getBytes("UTF-8"));
			BigInteger bigInt = new BigInteger(1,digest);
			String hashtext = bigInt.toString(16);
			// Now we need to zero pad it if you actually want the full 32 chars.
			while(hashtext.length() < 32 ){
			  hashtext = "0"+hashtext;
			}
			return hashtext;
		} catch (NoSuchAlgorithmException | UnsupportedEncodingException e) {
			e.printStackTrace();
		}
		return null;
	}
	
	
}
